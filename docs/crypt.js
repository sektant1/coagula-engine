/* ============================================================
 * CRYPT.SYS — runtime atmosphere & micro-interactions
 * Vanilla JS, no deps. doxygen-awesome plugin init lives in
 * the inline <script> in header.html (runs first).
 * ============================================================ */
(function () {
  'use strict';

  var REDUCED = window.matchMedia('(prefers-reduced-motion: reduce)').matches;
  var COARSE  = window.matchMedia('(pointer: coarse)').matches;

  function ready (fn) {
    if (document.readyState !== 'loading') fn();
    else document.addEventListener('DOMContentLoaded', fn);
  }

  /* -------------------------------------------------- drop-cap injector
   * Marks the first long paragraph after an h1 in .contents so CSS
   * ::first-letter can render the illuminated capital. Skips short
   * paragraphs and admonition boxes. */
  function injectDropCap () {
    var content = document.querySelector('div.contents');
    if (!content) return;
    var h1 = content.querySelector('h1, .header .title');
    var p  = h1
      ? findNextParagraph(h1)
      : content.querySelector(':scope > p, .textblock > p');
    if (!p) return;
    var text = (p.textContent || '').trim();
    if (text.length < 60) return;
    if (p.closest('dl.section, .memitem, table')) return;
    p.classList.add('has-dropcap');
  }
  function findNextParagraph (node) {
    var n = node.nextElementSibling;
    while (n) {
      if (n.tagName === 'P' && n.textContent.trim().length > 0) return n;
      var nested = n.querySelector && n.querySelector('p');
      if (nested) return nested;
      n = n.nextElementSibling;
    }
    return null;
  }

  /* -------------------------------------------------- reveal-on-scroll
   * IntersectionObserver — adds .is-revealed in batches with a tiny
   * stagger so groups feel orchestrated, not jittery. */
  function setupReveal () {
    if (REDUCED || !('IntersectionObserver' in window)) return;
    var targets = document.querySelectorAll(
      'h1, h2, .memitem, table.memberdecls, dl.section, div.contents > .textblock > p:first-of-type'
    );
    if (!targets.length) return;
    targets.forEach(function (el) { el.classList.add('crypt-reveal'); });

    var io = new IntersectionObserver(function (entries) {
      var visible = entries.filter(function (e) { return e.isIntersecting; });
      visible.forEach(function (e, i) {
        var el = e.target;
        setTimeout(function () { el.classList.add('is-revealed'); }, i * 55);
        io.unobserve(el);
      });
    }, { rootMargin: '0px 0px -8% 0px', threshold: 0.05 });

    targets.forEach(function (el) { io.observe(el); });
  }

  /* -------------------------------------------------- torch cursor glow
   * rAF-throttled mousemove driving a fixed radial-gradient div. */
  function setupTorch () {
    if (REDUCED || COARSE) return;
    var glow = document.querySelector('.crypt-cursor-glow');
    if (!glow) return;

    var x = 0, y = 0, pending = false, on = false;
    function paint () {
      pending = false;
      glow.style.transform = 'translate3d(' + (x - 180) + 'px,' + (y - 180) + 'px,0)';
    }
    window.addEventListener('mousemove', function (e) {
      x = e.clientX; y = e.clientY;
      if (!on) { glow.classList.add('is-on'); on = true; }
      if (!pending) { pending = true; requestAnimationFrame(paint); }
    }, { passive: true });

    document.addEventListener('mouseleave', function () {
      glow.classList.remove('is-on'); on = false;
    });
  }

  /* -------------------------------------------------- PSX hover jitter
   * Brief chromatic-aberration kick on links + member names. */
  function setupGlitch () {
    if (REDUCED) return;
    var sel = 'a.el, a.elRef, .memname, doxygen-awesome-fragment-copy-button';
    document.body.addEventListener('mouseenter', function (e) {
      var t = e.target;
      if (!(t instanceof Element)) return;
      if (!t.matches || !t.matches(sel)) return;
      if (t.classList.contains('psx-glitch')) return;
      t.classList.add('psx-glitch');
      setTimeout(function () { t.classList.remove('psx-glitch'); }, 260);
    }, true);
  }

  /* -------------------------------------------------- ember particles
   * Drifting embers from below, gentle horizontal sway, additive blend.
   * Capped count, density scales with viewport, paused when tab hidden
   * or viewport too small. */
  function setupEmbers () {
    if (REDUCED) return;
    var canvas = document.querySelector('.crypt-embers');
    if (!canvas) return;
    var ctx = canvas.getContext('2d', { alpha: true });
    if (!ctx) return;

    var dpr = Math.min(window.devicePixelRatio || 1, 2);
    var W = 0, H = 0;
    var embers = [];
    var raf = 0, running = false;

    function resize () {
      W = window.innerWidth;
      H = window.innerHeight;
      canvas.width  = Math.floor(W * dpr);
      canvas.height = Math.floor(H * dpr);
      canvas.style.width  = W + 'px';
      canvas.style.height = H + 'px';
      ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
      var target = Math.min(70, Math.round((W * H) / 32000));
      while (embers.length < target) embers.push(spawn(true));
      if (embers.length > target) embers.length = target;
    }

    function spawn (anywhere) {
      // mostly phosphor green, occasional brighter spark
      var amber = Math.random() < 0.05;
      var hue = amber ? 42 : 130;
      var sat = amber ? 90 : 95;
      var lit = amber ? 58 : 55 + Math.random() * 12;
      return {
        x: Math.random() * W,
        y: anywhere ? Math.random() * H : H + Math.random() * 40,
        r: 0.5 + Math.random() * 1.4,
        vy: -(0.10 + Math.random() * 0.40),
        vx: (Math.random() - 0.5) * 0.14,
        sway: 0.3 + Math.random() * 0.8,
        swayPh: Math.random() * Math.PI * 2,
        life: 0,
        max: 280 + Math.random() * 380,
        hue: hue, sat: sat, lit: lit,
        a: 0.0
      };
    }

    function step () {
      raf = 0;
      if (!running) return;
      ctx.clearRect(0, 0, W, H);
      ctx.globalCompositeOperation = 'lighter';

      for (var i = 0; i < embers.length; i++) {
        var e = embers[i];
        e.life++;
        e.swayPh += 0.012;
        e.x += e.vx + Math.sin(e.swayPh) * e.sway * 0.06;
        e.y += e.vy;

        // fade in / out across lifetime
        var t = e.life / e.max;
        if      (t < 0.15) e.a = (t / 0.15) * 0.85;
        else if (t > 0.75) e.a = (1 - (t - 0.75) / 0.25) * 0.85;
        else               e.a = 0.85;

        if (e.life > e.max || e.y < -20 || e.x < -20 || e.x > W + 20) {
          embers[i] = spawn(false);
          continue;
        }

        var col = 'hsla(' + e.hue + ',' + e.sat + '%,' + e.lit + '%,';
        // soft halo
        var g = ctx.createRadialGradient(e.x, e.y, 0, e.x, e.y, e.r * 6);
        g.addColorStop(0,   col + (e.a * 0.55) + ')');
        g.addColorStop(0.4, col + (e.a * 0.18) + ')');
        g.addColorStop(1,   col + '0)');
        ctx.fillStyle = g;
        ctx.beginPath();
        ctx.arc(e.x, e.y, e.r * 6, 0, Math.PI * 2);
        ctx.fill();
        // bright core
        ctx.fillStyle = col + (e.a) + ')';
        ctx.beginPath();
        ctx.arc(e.x, e.y, e.r, 0, Math.PI * 2);
        ctx.fill();
      }
      ctx.globalCompositeOperation = 'source-over';
      raf = requestAnimationFrame(step);
    }

    function start () {
      if (running) return;
      running = true;
      raf = requestAnimationFrame(step);
    }
    function stop () {
      running = false;
      if (raf) cancelAnimationFrame(raf);
      raf = 0;
      ctx.clearRect(0, 0, W, H);
    }

    resize();
    window.addEventListener('resize', resize, { passive: true });
    document.addEventListener('visibilitychange', function () {
      if (document.hidden) stop(); else start();
    });
    start();
  }

  /* -------------------------------------------------- search-box focus
   * doxygen renders a custom #MSearchBox; mark focused state for CSS. */
  function setupSearchFocus () {
    var box   = document.getElementById('MSearchBox');
    var field = document.getElementById('MSearchField');
    if (!box || !field) return;
    field.setAttribute('placeholder', 'scry…');
    field.addEventListener('focus', function () { box.classList.add('focused'); });
    field.addEventListener('blur',  function () { box.classList.remove('focused'); });
  }

  /* -------------------------------------------------- boot */
  ready(function () {
    injectDropCap();
    setupReveal();
    setupTorch();
    setupGlitch();
    setupEmbers();
    setupSearchFocus();
  });
})();
