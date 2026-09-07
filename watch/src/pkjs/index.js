var config = require("./config");
var SERVICE = config.origin(require("./env").service);
var state;
try {
  state = JSON.parse(localStorage.getItem("pond.v1"));
} catch (_) {}
if (!state) {
  var initial = config.defaults();
  initial.seed = Math.floor(Math.random() * 2147483646) + 1;
  initial.shape = initial.seed % 3;
  initial.palette = Math.floor(initial.seed / 3) % 2;
  state = { settings: initial, origin: SERVICE, applied: 0, counter: 0 };
}
// Canonicalise existing installations without changing their credentials.
state.origin = config.origin(state.origin);
var timer,
  busy = false,
  snapshot = true,
  failures = 0,
  started = false,
  configurationRequested = false;
var queue = [],
  sending = false,
  lastPreview = 0,
  lastPoll = 0;
function save() {
  localStorage.setItem("pond.v1", JSON.stringify(state));
}
function now() {
  return Math.floor(Date.now() / 1000);
}
function message(data, callback) {
  if (queue.length >= 4) {
    if (callback) callback(false);
    return;
  }
  queue.push({ data: data, callback: callback });
  drain();
}
function drain() {
  if (sending || !queue.length) return;
  sending = true;
  var item = queue.shift();
  function done(ok) {
    sending = false;
    if (item.callback) item.callback(ok);
    drain();
  }
  Pebble.sendAppMessage(
    item.data,
    function () {
      done(true);
    },
    function () {
      snapshot = true;
      done(false);
    }
  );
}
function syncSettings() {
  var s = state.settings;
  message({
    Type: 1,
    Revision: s.revision,
    Joined: s.joined ? 1 : 0,
    Gentle: s.gentle ? 1 : 0,
    QuietStart: s.quietStart,
    QuietEnd: s.quietEnd,
    AllDay: s.allDay ? 1 : 0,
    PauseUntil: s.pauseUntil,
    Reduced: s.reduced ? 1 : 0,
    Tick: s.tick ? 1 : 0,
    Seed: s.seed,
    Shape: s.shape,
    Palette: s.palette
  });
}
function deliverReceipt(receipt, attempt) {
  if (!state.settings.joined || state.deleting ||
      state.lastReceipt !== receipt || now() >= receipt.expires ||
      now() < receipt.created) return;
  message({ Type: 5, Request: receipt.watchId,
    Result: attempt && receipt.result === 1 ? 2 : receipt.result }, function (ok) {
    if (!ok && attempt < 3) {
      setTimeout(function () { deliverReceipt(receipt, attempt + 1); }, 1000);
    }
  });
}
function finishDrop(pending, result) {
  state.pending = null;
  state.lastReceipt = { watchId: pending.watchId, result: result,
    created: now(), expires: now() + 120 };
  save();
  deliverReceipt(state.lastReceipt, 0);
}
function request(method, path, body, callback, authenticated) {
  busy = true;
  var xhr = new XMLHttpRequest(),
    finished = false,
    origin = state.origin;
  function done(status, data) {
    if (finished) return;
    finished = true;
    busy = false;
    if (xhr.responseURL && !config.sameOrigin(xhr.responseURL, origin)) {
      status = 0;
      data = null;
    }
    callback(status, data);
  }
  xhr.open(method, origin + path, true);
  xhr.timeout = 12000;
  // Credentials never appear in URLs or request bodies. Browser XHR strips Authorization
  // on cross-origin redirects; additionally reject a changed response origin.
  if (authenticated !== false)
    xhr.setRequestHeader("Authorization", "Bearer " + state.credential);
  if (body) xhr.setRequestHeader("Content-Type", "application/json");
  xhr.onload = function () {
    var data;
    try {
      data = JSON.parse(xhr.responseText);
    } catch (_) {}
    done(xhr.status, data);
  };
  xhr.onerror = xhr.ontimeout = function () {
    done(0, null);
  };
  xhr.send(body ? JSON.stringify(body) : null);
}
function schedule(delay) {
  clearTimeout(timer);
  timer = setTimeout(work, delay);
}
function failed() {
  snapshot = true;
  failures = Math.min(failures + 1, 5);
  schedule(Math.min(60000 * Math.pow(2, failures), 900000));
}
function expired() {
  state.settings.joined = false;
  state.settings.gentle = false;
  state.settings.revision++;
  state.credential = null;
  state.registered = false;
  state.pending = null;
  state.error = "Rejoin through settings";
  save();
  syncSettings();
  schedule(60000);
}
function work() {
  if (busy) {
    schedule(1000);
    return;
  }
  if (state.applied !== state.settings.revision) syncSettings();
  if (configurationRequested) {
    configurationRequested = false;
    request("HEAD", "/settings.html", null, function (status) {
      openConfiguration(status === 200 ? state.origin : SERVICE, status !== 200);
      schedule(0);
    }, false);
    return;
  }
  if (state.deleting) {
    request("DELETE", "/v1/installation", null, function (status, data) {
      if (status === 200 && data && data.deleted) {
        state.deleting = false;
        state.credential = null;
        state.registered = false;
        state.deleted = true;
        save();
        schedule(60000);
      } else failed();
    });
    return;
  }
  if (!state.settings.joined || !state.credential) {
    schedule(60000);
    return;
  }
  if (!state.registered) {
    request(
      "POST",
      "/v1/installations",
      { version: 1 },
      function (status, data) {
        if (status === 200 && data && data.id) {
          state.registered = true;
          failures = 0;
          save();
          schedule(0);
        } else if (status === 410 || status === 401) expired();
        else failed();
      }
    );
    return;
  }
  if (state.pending) {
    var pending = state.pending;
    if (now() - pending.created >= 120 || now() < pending.created) {
      finishDrop(pending, 3);
      schedule(0);
      return;
    }
    request(
      "POST",
      "/v1/drops",
      {
        version: 1,
        request_id: pending.id,
        source_bundle: pending.source || null
      },
      function (status, data) {
        // Leaving while a request is in flight must not restore participation or tick.
        if (!state.settings.joined || state.deleting) {
          schedule(0);
          return;
        }
        if (
          status === 200 &&
          data &&
          data.request_id === pending.id &&
          data.accepted_at
        ) {
          finishDrop(pending,
            !pending.retried && !snapshot && !data.duplicate ? 1 : 2);
          failures = 0;
          schedule(0);
        } else if (status === 401) expired();
        else if (status >= 400 && status < 500) {
          // A definitive rejection is different from a lost response.
          // After a previous uncertain attempt, acceptance is still possible.
          finishDrop(pending, pending.retried ? 3 : status === 429 ? 4 : 5);
          schedule(60000);
        } else {
          pending.retried = true;
          save();
          message({ Type: 5, Request: pending.watchId, Result: 0 });
          snapshot = true;
          schedule(10000);
        }
      }
    );
    return;
  }
  if (!lastPoll || now() - lastPoll > 120 || now() < lastPoll) snapshot = true;
  var wasSnapshot = snapshot;
  request(
    "GET",
    "/v1/pond" +
      (!wasSnapshot && state.cursor
        ? "?after=" + encodeURIComponent(state.cursor)
        : ""),
    null,
    function (status, data) {
      if (!state.settings.joined || state.deleting) {
        schedule(0);
        return;
      }
      if (status === 401) {
        expired();
        return;
      }
      if (status === 422) {
        state.cursor = null;
        save();
        failed();
        return;
      }
      if (status !== 200 || !data || data.version !== 1 || !data.server_time) {
        failed();
        return;
      }
      failures = 0;
      lastPoll = now();
      state.cursor = data.cursor;
      save();
      var b = data.bundle;
      message(
        {
          Type: 2,
          ServerTime: data.server_time,
          Snapshot: wasSnapshot || snapshot ? 1 : 0,
          Bundle: b ? b.id : 0,
          Expires: b ? b.expires_at : 0,
          Other: b && b.has_other ? 1 : 0,
          Echo: b && b.echo_answer ? 1 : 0
        },
        function (ok) {
          snapshot = !ok;
        }
      );
      schedule(
        state.pending || state.deleting
          ? 0
          : 55000 + Math.floor(Math.random() * 10000)
      );
    }
  );
}
function uuid(counter) {
  // Prefix is generated with browser crypto at explicit join; counter persists before send.
  var hex =
    state.requestSeed.slice(0, 24) +
    ("00000000" + counter.toString(16)).slice(-8);
  hex = hex.slice(0, 12) + "4" + hex.slice(13);
  hex =
    hex.slice(0, 16) +
    ((parseInt(hex.charAt(16), 16) & 3) | 8).toString(16) +
    hex.slice(17);
  return (
    hex.slice(0, 8) +
    "-" +
    hex.slice(8, 12) +
    "-" +
    hex.slice(12, 16) +
    "-" +
    hex.slice(16, 20) +
    "-" +
    hex.slice(20)
  );
}
Pebble.addEventListener("ready", function () {
  started = true;
  state.applied = 0;
  state.canPreview = false;
  snapshot = true;
  if (state.pending) state.pending.retried = true;
  save();
  syncSettings();
  if (state.lastReceipt) deliverReceipt(state.lastReceipt, 1);
  schedule(0);
});
Pebble.addEventListener("appmessage", function (e) {
  var p = e.payload;
  if (p.Type === 4 && p.Applied === state.settings.revision) {
    state.applied = p.Applied;
    state.canPreview = p.Preview === 1;
    if (typeof p.PauseUntil === "number")
      state.settings.pauseUntil = p.PauseUntil;
    save();
  }
  if (
    p.Type === 3 &&
    state.settings.joined &&
    !state.deleting &&
    p.Request > 0
  ) {
    if (state.pending) {
      if (state.pending.watchId === p.Request) schedule(0);
      return;
    }
    // Remember the last watch request so AppMessage redelivery cannot create a second ID.
    if (state.lastWatchId === p.Request) {
      if (state.lastReceipt && state.lastReceipt.watchId === p.Request)
        deliverReceipt(state.lastReceipt, 1);
      return;
    }
    state.lastReceipt = null;
    state.counter++;
    state.lastWatchId = p.Request;
    state.pending = {
      id: uuid(state.counter),
      watchId: p.Request,
      source: p.Source || null,
      created: now(),
      retried: false
    };
    save();
    schedule(0);
  }
});
function openConfiguration(origin, recovery) {
  var view = {
    settings: state.settings,
    origin: state.origin,
    applied: state.applied,
    canPreview: !!state.canPreview,
    deleting: !!state.deleting,
    deleted: !!state.deleted,
    registered: !!state.registered,
    error: state.error || "",
    configurationFallback: !!recovery
  };
  Pebble.openURL(
    origin + "/settings.html#" + encodeURIComponent(JSON.stringify(view))
  );
}
Pebble.addEventListener("showConfiguration", function () {
  if (state.origin === SERVICE) openConfiguration(state.origin, false);
  else {
    configurationRequested = true;
    schedule(0);
  }
});
Pebble.addEventListener("webviewclosed", function (e) {
  if (!e.response || e.response === "CANCELLED") return;
  try {
    var result = JSON.parse(decodeURIComponent(e.response));
    if (result.action === "preview") {
      if (
        started &&
        !sending &&
        !queue.length &&
        state.applied === state.settings.revision &&
        state.settings.gentle &&
        now() - lastPreview >= 10
      ) {
        lastPreview = now();
        message({ Type: 6 });
      }
      return;
    }
    var s = config.settings(result.settings),
      origin = config.origin(result.origin);
    if (origin !== state.origin && (state.credential || state.deleting))
      throw new Error("Leave the old pond before changing service");
    if (result.action === "join") {
      if (
        state.credential ||
        !/^[0-9a-f]{64}$/.test(result.credential) ||
        !/^[0-9a-f]{32}$/.test(result.requestSeed)
      )
        throw new Error("Invalid join");
      state.credential = result.credential;
      state.requestSeed = result.requestSeed;
      state.registered = false;
      state.counter = 0;
      state.lastWatchId = null;
      state.lastReceipt = null;
      state.deleted = false;
      state.cursor = null;
      s.joined = true;
      s.gentle = false;
    } else if (result.action === "leave") {
      s.joined = false;
      s.gentle = false;
      state.deleting = !!state.credential;
      state.pending = null;
      state.lastReceipt = null;
    } else if (result.action !== "save" || s.joined !== state.settings.joined)
      throw new Error("Invalid action");
    s.revision = state.settings.revision + 1;
    state.settings = s;
    state.origin = origin;
    state.error = "";
    save();
    snapshot = true;
    queue = [];
    syncSettings();
    schedule(0);
  } catch (_) {
    state.error = "Settings were not changed. Reopen to try again.";
    save();
  }
});
