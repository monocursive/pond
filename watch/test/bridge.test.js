"use strict";
const test = require("node:test"),
  assert = require("node:assert/strict"),
  fs = require("node:fs"),
  vm = require("node:vm");
const config = require("../src/pkjs/config");
function bridge(saved) {
  let milliseconds = Date.now();
  let stored = saved ? JSON.stringify(saved) : null,
    timers = [],
    requests = [],
    messages = [],
    listeners = {},
    opened,
    failMessage = false;
  const localStorage = {
    getItem() {
      return stored;
    },
    setItem(k, v) {
      stored = v;
    },
  };
  function XHR() {
    this.headers = {};
    requests.push(this);
  }
  XHR.prototype.open = function (method, url) {
    this.method = method;
    this.url = url;
  };
  XHR.prototype.setRequestHeader = function (k, v) {
    this.headers[k] = v;
  };
  XHR.prototype.send = function (body) {
    this.body = body;
  };
  XHR.prototype.respond = function (status, data, responseURL) {
    this.status = status;
    this.responseText = JSON.stringify(data);
    this.responseURL = responseURL || new URL(this.url).href;
    this.onload();
  };
  const context = {
    localStorage,
    XMLHttpRequest: XHR,
    require(p) {
      return p === "./config" ? config : { service: "http://127.0.0.1:4000" };
    },
    Date: { now() { return milliseconds; } },
    Math,
    JSON,
    setTimeout(fn, delay) {
      const t = { fn, delay };
      timers.push(t);
      return t;
    },
    clearTimeout(t) {
      timers = timers.filter((x) => x !== t);
    },
    Pebble: {
      addEventListener(k, f) {
        listeners[k] = f;
      },
      sendAppMessage(m, ok, fail) {
        messages.push(m);
        if (failMessage) {
          failMessage = false;
          fail();
        } else ok();
      },
      openURL(url) {
        opened = url;
      },
    },
  };
  vm.runInNewContext(
    fs.readFileSync(require.resolve("../src/pkjs/index"), "utf8"),
    context,
  );
  return {
    failNextMessage() { failMessage = true; },
    advance(seconds) { milliseconds += seconds * 1000; },
    listeners,
    requests,
    messages,
    get state() {
      return JSON.parse(stored);
    },
    get opened() {
      return opened;
    },
    run() {
      const next = timers.shift();
      assert.ok(next, "scheduled work");
      next.fn();
    },
  };
}
function joined() {
  return {
    origin: "https://pond.example",
    credential: "a".repeat(64),
    requestSeed: "b".repeat(32),
    settings: { ...config.defaults(), joined: true, seed: 4 },
    applied: 1,
    counter: 0,
    registered: true,
  };
}
test("unjoined launch never registers or polls; configuration contains no credential", () => {
  const b = bridge();
  b.listeners.ready();
  b.run();
  assert.equal(b.requests.length, 0);
  b.listeners.showConfiguration();
  assert.ok(b.opened.includes("settings.html#"));
});
test("first snapshot is silent and live polling uses cursor", () => {
  const b = bridge(joined());
  b.listeners.ready();
  b.run();
  b.requests[0].respond(200, {
    version: 1,
    server_time: 100,
    cursor: "cursor",
    bundle: { id: 3, expires_at: 200, has_other: true },
  });
  assert.equal(b.messages.at(-1).Snapshot, 1);
  b.run();
  assert.ok(b.requests[1].url.endsWith("?after=cursor"));
  b.requests[1].respond(200, {
    version: 1,
    server_time: 160,
    cursor: "next",
    bundle: { id: 4, expires_at: 260, has_other: true },
  });
  assert.equal(b.messages.at(-1).Snapshot, 0);
});
test("unknown drop retries same UUID and never creates delayed feedback", () => {
  const b = bridge(joined());
  b.listeners.ready();
  b.listeners.appmessage({ payload: { Type: 3, Request: 9, Source: 22 } });
  b.run();
  const body = b.requests[0].body;
  assert.equal(JSON.parse(body).source_bundle, 22);
  b.requests[0].respond(0, null);
  b.run();
  assert.equal(b.requests[1].body, body);
  b.requests[1].respond(200, {
    request_id: JSON.parse(body).request_id,
    accepted_at: 100,
    duplicate: true,
  });
  assert.equal(b.messages.at(-1).Result, 2);
  assert.equal(b.state.pending, null);
  b.listeners.appmessage({ payload: { Type: 3, Request: 9 } });
  assert.equal(b.state.pending, null);
});
test("no concurrent network requests and expired pending actions are discarded", () => {
  const s = joined();
  s.pending = { id: "old", watchId: 3, created: 1 };
  const b = bridge(s);
  b.listeners.ready();
  b.run();
  assert.equal(b.requests.length, 0);
  assert.equal(b.state.pending, null);
  b.run();
  b.listeners.appmessage({ payload: { Type: 3, Request: 4 } });
  b.run();
  assert.equal(b.requests.length, 1);
});
test("settings are applied only after matching watch acknowledgement", () => {
  const b = bridge(joined());
  b.listeners.ready();
  assert.equal(b.state.applied, 0);
  b.listeners.appmessage({ payload: { Type: 4, Applied: 1 } });
  b.listeners.webviewclosed({
    response: encodeURIComponent(
      JSON.stringify({
        action: "save",
        settings: { ...joined().settings, gentle: true },
        origin: "https://pond.example",
      }),
    ),
  });
  assert.equal(b.state.settings.revision, 2);
  assert.equal(b.state.applied, 1);
  b.listeners.appmessage({ payload: { Type: 4, Applied: 1 } });
  assert.equal(b.state.applied, 1);
  b.listeners.appmessage({ payload: { Type: 4, Applied: 2 } });
  assert.equal(b.state.applied, 2);
});
test("leave stops polls, retains credential for deletion and needs server acknowledgement", () => {
  const b = bridge(joined());
  b.listeners.ready();
  b.listeners.webviewclosed({
    response: encodeURIComponent(
      JSON.stringify({
        action: "leave",
        settings: joined().settings,
        origin: "https://pond.example",
      }),
    ),
  });
  assert.equal(b.state.settings.joined, false);
  assert.equal(b.state.deleting, true);
  b.run();
  assert.equal(b.requests[0].method, "DELETE");
  b.requests[0].respond(0, null);
  assert.ok(b.state.credential);
  b.run();
  b.requests[1].respond(200, { deleted: true });
  assert.equal(b.state.credential, null);
  assert.equal(b.state.deleting, false);
});
test("endpoint changes cannot carry credentials and invalid origins are rejected", () => {
  for (const url of [
    "http://evil.test",
    "https://good.test@evil.test",
    "https://good.test/path",
    "https://good.test?x",
  ])
    assert.throws(() => config.origin(url));
  const b = bridge(joined());
  b.listeners.ready();
  b.listeners.webviewclosed({
    response: encodeURIComponent(
      JSON.stringify({
        action: "save",
        settings: joined().settings,
        origin: "https://other.test",
      }),
    ),
  });
  assert.equal(b.state.origin, "https://pond.example");
  b.listeners.showConfiguration();
  b.run();
  b.requests.at(-1).respond(200, null);
  assert.equal(decodeURIComponent(b.opened).includes("a".repeat(64)), false);
});

test("a suspended runtime resumes with a silent snapshot", () => {
  const b=bridge(joined()); b.listeners.ready(); b.run();
  b.requests[0].respond(200,{version:1,server_time:100,cursor:"old",bundle:null});
  b.advance(180); b.run();
  assert.ok(b.requests[1].url.endsWith("/v1/pond"));
  b.requests[1].respond(200,{version:1,server_time:280,cursor:"new",bundle:{id:4,expires_at:400,has_other:true}});
  assert.equal(b.messages.at(-1).Snapshot,1);
});

test("credential expiry retries a lost participation-off update without more API activity", () => {
  const b = bridge(joined());
  b.listeners.ready();
  b.run();
  b.failNextMessage();
  b.requests[0].respond(401, { version: 1, error: "unauthorized" });
  assert.equal(b.state.settings.joined, false);
  assert.equal(b.state.credential, null);
  b.run();
  assert.equal(b.requests.length, 1);
  assert.equal(b.messages.at(-1).Type, 1);
  assert.equal(b.messages.at(-1).Joined, 0);
  b.listeners.appmessage({ payload: { Type: 4, Applied: 2 } });
  const count = b.messages.length;
  b.run();
  assert.equal(b.messages.length, count);
});

test("canonical origins preserve credentials and accept successful join/deletion responses", () => {
  for (const origin of ["https://POND.example", "https://pond.example:00443/"]) {
    const s = joined(); s.origin = origin; s.registered = false;
    const b = bridge(s); b.listeners.ready(); b.run();
    assert.equal(b.state.origin, "https://pond.example");
    b.requests[0].respond(200, { version: 1, id: "installation" });
    assert.equal(b.state.registered, true);
    assert.equal(b.state.credential, s.credential);
    b.listeners.webviewclosed({ response: encodeURIComponent(JSON.stringify({
      action: "leave", settings: s.settings, origin
    })) });
    b.run(); b.requests[1].respond(200, { version: 1, deleted: true });
    assert.equal(b.state.deleting, false);
    assert.equal(b.state.credential, null);
  }
  assert.equal(config.origin("HTTP://LOCALHOST:080/"), "http://localhost");
  for (const origin of ["https://pond.example:99999", "https://pond.example:0"])
    assert.throws(() => config.origin(origin));
  assert.equal(config.sameOrigin("https://pond.example.evil.test/v1/pond", "https://pond.example"), false);
  assert.equal(config.sameOrigin("https://pond.example@evil.test/v1/pond", "https://pond.example"), false);
  assert.equal(config.sameOrigin("http://pond.example/v1/pond", "https://pond.example"), false);
  const b = bridge(joined()); b.listeners.ready(); b.run();
  b.requests[0].respond(200, { version: 1, server_time: 100, cursor: "bad" }, "https://evil.test/v1/pond");
  assert.equal(b.messages.some(m => m.Type === 2), false);
});

test("custom settings use the selected host without sending credentials or overlapping HTTP", () => {
  const b = bridge(joined()); b.listeners.ready(); b.run();
  b.listeners.showConfiguration(); b.run();
  assert.equal(b.requests.length, 1);
  b.requests[0].respond(200, { version: 1, server_time: 100, cursor: "one", bundle: null });
  b.run();
  const probe = b.requests[1];
  assert.equal(probe.method, "HEAD");
  assert.equal(probe.url, "https://pond.example/settings.html");
  assert.equal(probe.headers.Authorization, undefined);
  probe.respond(200, null);
  assert.equal(new URL(b.opened).origin, "https://pond.example");
  assert.equal(decodeURIComponent(b.opened).includes(b.state.credential), false);
});

test("unavailable custom settings fall back without moving identity or claiming deletion", () => {
  for (const outcome of ["timeout", "missing", "redirect"]) {
    const b = bridge(joined()); b.listeners.ready(); b.listeners.showConfiguration(); b.run();
    const probe = b.requests[0];
    if (outcome === "timeout") probe.ontimeout();
    else probe.respond(outcome === "missing" ? 404 : 200, null, "https://elsewhere.test/settings.html");
    assert.equal(new URL(b.opened).origin, "http://127.0.0.1:4000");
    const view = JSON.parse(decodeURIComponent(new URL(b.opened).hash.slice(1)));
    assert.equal(view.configurationFallback, true);
    assert.equal(view.origin, "https://pond.example");
    assert.equal(view.credential, undefined);
    b.listeners.webviewclosed({ response: encodeURIComponent(JSON.stringify({
      action: "leave", settings: view.settings, origin: view.origin
    })) });
    b.run();
    assert.equal(b.requests[1].url, "https://pond.example/v1/installation");
    assert.equal(b.requests[1].headers.Authorization, "Bearer " + joined().credential);
    b.requests[1].respond(0, null);
    assert.equal(b.state.deleting, true);
    assert.equal(b.state.credential, joined().credential);
  }
});
