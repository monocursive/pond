(function () {
  "use strict";
  var view = {
    settings: {
      revision: 1,
      joined: false,
      gentle: false,
      quietStart: 1320,
      quietEnd: 480,
      allDay: false,
      pauseUntil: 0,
      reduced: false,
      tick: false,
      seed: 1,
      shape: 0,
      palette: 0,
    },
    origin: location.origin,
    applied: 0,
  };
  try {
    if (location.hash.length > 1)
      view = JSON.parse(decodeURIComponent(location.hash.slice(1)));
  } catch (_) {}
  // Remove device preferences from browser history after reading; no credentials arrive here.
  history.replaceState(null, "", location.pathname);
  var s = view.settings;
  function el(id) {
    return document.getElementById(id);
  }
  function time(n) {
    return (
      ("0" + Math.floor(n / 60)).slice(-2) + ":" + ("0" + (n % 60)).slice(-2)
    );
  }
  function minutes(v) {
    var parts = v.split(":");
    return +parts[0] * 60 + +parts[1];
  }
  function randomHex(bytes) {
    var data = new Uint8Array(bytes);
    crypto.getRandomValues(data);
    return Array.from(data, function (v) {
      return ("0" + v.toString(16)).slice(-2);
    }).join("");
  }
  el("mode").value = s.gentle ? "gentle" : "silent";
  el("quietStart").value = time(s.quietStart);
  el("quietEnd").value = time(s.quietEnd);
  ["tick", "allDay", "reduced"].forEach(function (k) {
    el(k).checked = s[k];
  });
  el("shape").value = s.shape;
  el("palette").value = s.palette;
  el("origin").value = view.origin;
  el("origin").disabled = !!(s.joined || view.deleting);
  el("join").hidden = s.joined || view.deleting;
  el("leave").hidden = !s.joined;
  if (!s.joined) el("save").textContent = "Save local settings";
  el("mode").disabled = !s.joined;
  function previewState() {
    el("preview").disabled =
      !s.joined ||
      !view.canPreview ||
      !s.gentle ||
      el("mode").value !== "gentle" ||
      view.applied !== s.revision;
  }
  previewState();
  el("mode").onchange = previewState;
  if (view.deleting) {
    el("status").textContent = "Leaving the pond";
    el("detail").textContent =
      "Server deletion: waiting. Watch: " +
      (view.applied === s.revision
        ? "participation off."
        : "waiting for acknowledgement.");
  } else if (s.joined) {
    el("status").textContent =
      view.applied === s.revision
        ? (s.gentle ? "Gentle" : "Silent") + " · applied on watch"
        : "Waiting for watch";
    el("detail").textContent = view.registered
      ? "There is no need to keep checking. Pond stays quiet between ripples."
      : "Joining the service. Reopen to check the result.";
  } else if (view.deleted) {
    el("status").textContent = "Server data deleted";
    el("detail").textContent =
      view.applied === s.revision
        ? "Participation is off on your watch. Your local face remains."
        : "Waiting for the watch to stop participation. Switch faces for immediate quiet.";
  }
  if (view.error) el("error").textContent = view.error;
  if (view.configurationFallback) {
    el("recovery").hidden = false;
    el("recovery").textContent =
      "Your pond is unavailable. This recovery page can save comfort settings or request deletion. Deletion stays pending until your pond returns; your service has not changed.";
  }
  function collect() {
    var result = Object.assign({}, s),
      pause = el("pause").value;
    result.gentle = el("mode").value === "gentle";
    result.quietStart = minutes(el("quietStart").value);
    result.quietEnd = minutes(el("quietEnd").value);
    ["tick", "allDay", "reduced"].forEach(function (k) {
      result[k] = el(k).checked;
    });
    result.shape = +el("shape").value;
    result.palette = +el("palette").value;
    if (pause === "hour") result.pauseUntil = -3;
    else if (pause === "tomorrow") result.pauseUntil = -2;
    else if (pause !== "keep") result.pauseUntil = +pause;
    return result;
  }
  function close(action) {
    if (!el("settings").reportValidity()) return;
    var result = {
      action: action,
      settings: collect(),
      origin: el("origin").value,
    };
    try {
      if (action === "join") {
        result.credential = randomHex(32);
        result.requestSeed = randomHex(16);
        result.settings.seed = (parseInt(randomHex(4), 16) % 2147483646) + 1;
      }
      location.href =
        "pebblejs://close#" + encodeURIComponent(JSON.stringify(result));
    } catch (_) {
      el("error").textContent =
        "Secure random generation is unavailable. Open settings through HTTPS to join.";
    }
  }
  el("settings").onsubmit = function (e) {
    e.preventDefault();
    close("save");
  };
  el("join").onclick = function () {
    close("join");
  };
  el("preview").onclick = function () {
    close("preview");
  };
  el("leave").onclick = function () {
    if (
      confirm(
        "Leave the pond and delete your linked server data? Your local watchface stays.",
      )
    )
      close("leave");
  };
  el("shuffle").onclick = function () {
    el("shape").value = (+el("shape").value + 1) % 3;
    el("palette").value = (+el("palette").value + 1) % 2;
  };
})();
