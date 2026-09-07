/* Plain ES5: shared validation for the bridge and Node tests. */
exports.defaults = function () {
  return {
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
    palette: 0
  };
};
exports.origin = function (value) {
  if (typeof value !== "string" || value.length > 200)
    throw new Error("Invalid service");
  var match = /^(https?):\/\/([a-z0-9](?:[a-z0-9.-]*[a-z0-9])?)(?::([0-9]{1,5}))?\/?$/i.exec(value);
  if (!match) throw new Error("Use an HTTPS origin");
  var scheme = match[1].toLowerCase(),
    host = match[2].toLowerCase(),
    port = match[3] ? Number(match[3]) : null;
  if (scheme !== "https" && host !== "127.0.0.1" && host !== "localhost")
    throw new Error("Use an HTTPS origin");
  if (port !== null && (port < 1 || port > 65535))
    throw new Error("Invalid port");
  if (port === (scheme === "https" ? 443 : 80)) port = null;
  return scheme + "://" + host + (port === null ? "" : ":" + port);
};
exports.sameOrigin = function (url, origin) {
  var match = typeof url === "string" && /^(https?:\/\/[^/?#]+)(?:[/?#]|$)/i.exec(url);
  try {
    return !!match && exports.origin(match[1]) === exports.origin(origin);
  } catch (_) {
    return false;
  }
};
exports.settings = function (input) {
  var s = exports.defaults();
  ["joined", "gentle", "allDay", "reduced", "tick"].forEach(function (k) {
    if (typeof input[k] !== "boolean") throw new Error("Invalid setting");
    s[k] = input[k];
  });
  var bounds = {
    quietStart: [0, 1439],
    quietEnd: [0, 1439],
    pauseUntil: [-3, 2147483647],
    seed: [1, 2147483647],
    shape: [0, 2],
    palette: [0, 1]
  };
  Object.keys(bounds).forEach(function (k) {
    var v = input[k],
      b = bounds[k];
    if (typeof v !== "number" || v % 1 || v < b[0] || v > b[1])
      throw new Error("Invalid setting");
    s[k] = v;
  });
  return s;
};
