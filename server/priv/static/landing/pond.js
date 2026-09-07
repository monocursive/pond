/* A local illustration, never a request to the shared pond. */
(function () {
  'use strict';
  var scene = document.getElementById('pond-scene');
  var button = document.querySelector('.plop-button');
  var status = document.getElementById('demo-status');
  var still = document.querySelector('.watch-still');
  var ripple = document.querySelector('.watch-ripple');
  var toggle = document.getElementById('motion-toggle');
  var preference = window.matchMedia('(prefers-reduced-motion: reduce)');
  var paused = preference.matches;
  var timer;
  var messages = ['A tiny hello, illustrated. Somewhere, a wrist might smile.', 'An excellent plop. Very round. No notes.', 'Another little hello. The sandwich can wait.'];
  var drops = 0;

  function settle() {
    window.clearTimeout(timer);
    scene.classList.remove('is-plopping');
    button.disabled = false;
  }

  function motion() {
    document.documentElement.dataset.motion = paused ? 'off' : 'on';
    toggle.setAttribute('aria-pressed', String(paused));
    toggle.textContent = preference.matches ? 'Reduced motion is on' : (paused ? 'Resume motion' : 'Pause motion');
    toggle.disabled = preference.matches;
    if (paused) settle();
  }

  toggle.hidden = false;
  button.disabled = false;
  motion();
  toggle.addEventListener('click', function () {
    paused = !paused;
    // The operating-system preference always wins over decorative motion.
    if (preference.matches) paused = true;
    motion();
  });
  preference.addEventListener('change', function (event) {
    paused = event.matches;
    motion();
  });
  button.addEventListener('click', function () {
    settle();
    still.hidden = true;
    ripple.hidden = false;
    status.textContent = messages[drops % messages.length];
    drops += 1;
    if (!paused && !preference.matches) {
      scene.classList.add('is-plopping');
      button.disabled = true;
      timer = window.setTimeout(settle, 2000);
    }
  });
  document.addEventListener('visibilitychange', function () {
    if (document.hidden) settle();
  });
}());
