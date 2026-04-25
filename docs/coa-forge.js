/* =====================================================================
   Coagula Engine docs — forge atmosphere script
   - Floating embers/cinders rising from beneath the page
   - Subtle parallax on the runic banner sigil
   - Code-block "ignite" hover glow
   - Reveal-on-scroll for content blocks (cheap IntersectionObserver)
   No dependencies. Bails out early on prefers-reduced-motion.
   ===================================================================== */
(function () {
  "use strict";

  var prefersReduce = window.matchMedia &&
    window.matchMedia("(prefers-reduced-motion: reduce)").matches;

  /* -------------------- Embers -------------------- */
  var canvas = document.getElementById("coa-embers");
  if (canvas && !prefersReduce) {
    var ctx = canvas.getContext("2d");
    var dpr = Math.min(window.devicePixelRatio || 1, 2);
    var W = 0, H = 0;
    var embers = [];
    var EMBER_TARGET = 36;

    function resize() {
      W = canvas.clientWidth = window.innerWidth;
      H = canvas.clientHeight = window.innerHeight;
      canvas.width  = W * dpr;
      canvas.height = H * dpr;
      ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
    }
    window.addEventListener("resize", resize, { passive: true });
    resize();

    function spawn(initial) {
      return {
        x: Math.random() * W,
        y: initial ? Math.random() * H : H + 8 + Math.random() * 40,
        vx: (Math.random() - 0.5) * 0.25,
        vy: -(0.18 + Math.random() * 0.55),
        r:  0.6 + Math.random() * 1.8,
        life: 0,
        max: 320 + Math.random() * 600,
        hue: 16 + Math.random() * 24,           /* 16–40°: deep ember to gold */
        sway: Math.random() * Math.PI * 2,
        swaySpeed: 0.005 + Math.random() * 0.012
      };
    }
    for (var i = 0; i < EMBER_TARGET; i++) embers.push(spawn(true));

    var lastT = performance.now();
    function tick(now) {
      var dt = Math.min(34, now - lastT);
      lastT = now;
      ctx.clearRect(0, 0, W, H);
      ctx.globalCompositeOperation = "lighter";

      for (var i = 0; i < embers.length; i++) {
        var e = embers[i];
        e.life += dt;
        e.sway += e.swaySpeed * dt;
        e.x += e.vx + Math.sin(e.sway) * 0.18;
        e.y += e.vy * (dt / 16);

        var t = e.life / e.max;
        if (t >= 1 || e.y < -10 || e.x < -10 || e.x > W + 10) {
          embers[i] = spawn(false);
          continue;
        }
        var alpha = (1 - t) * (0.55 + 0.45 * Math.sin(e.life * 0.01));
        var rad = e.r * (1 - t * 0.4);

        var grad = ctx.createRadialGradient(e.x, e.y, 0, e.x, e.y, rad * 6);
        grad.addColorStop(0,    "hsla(" + e.hue + ", 95%, 65%, " + alpha + ")");
        grad.addColorStop(0.4,  "hsla(" + (e.hue - 4) + ", 90%, 50%, " + (alpha * 0.5) + ")");
        grad.addColorStop(1,    "hsla(" + (e.hue - 8) + ", 85%, 30%, 0)");
        ctx.fillStyle = grad;
        ctx.beginPath();
        ctx.arc(e.x, e.y, rad * 6, 0, Math.PI * 2);
        ctx.fill();

        ctx.fillStyle = "hsla(" + (e.hue + 6) + ", 95%, 80%, " + (alpha * 0.9) + ")";
        ctx.beginPath();
        ctx.arc(e.x, e.y, rad, 0, Math.PI * 2);
        ctx.fill();
      }
      requestAnimationFrame(tick);
    }
    requestAnimationFrame(tick);
  }

  /* -------------------- Sigil parallax + flicker on scroll -------------------- */
  var sigil = document.querySelector(".coa-banner__sigil svg");
  if (sigil && !prefersReduce) {
    window.addEventListener("scroll", function () {
      var s = Math.min(40, window.scrollY * 0.04);
      sigil.style.transform = "translateY(" + s + "px) rotate(" + (s * 0.6) + "deg)";
    }, { passive: true });
  }

  /* -------------------- Code blocks: ignite on hover -------------------- */
  document.addEventListener("mousemove", function (ev) {
    var t = ev.target;
    if (!t || !t.closest) return;
    var frag = t.closest("div.fragment, pre.fragment");
    if (!frag) return;
    var rect = frag.getBoundingClientRect();
    var x = ((ev.clientX - rect.left) / rect.width) * 100;
    var y = ((ev.clientY - rect.top)  / rect.height) * 100;
    frag.style.backgroundImage =
      "radial-gradient(circle at " + x + "% " + y + "%, rgba(216,99,42,0.10), transparent 55%)," +
      "linear-gradient(180deg, #100b08 0%, #0a0705 100%)";
  });
  document.addEventListener("mouseleave", function (ev) {
    if (!ev.target || !ev.target.querySelectorAll) return;
    var frags = document.querySelectorAll("div.fragment, pre.fragment");
    frags.forEach(function (f) { f.style.backgroundImage = ""; });
  }, true);

  /* -------------------- Reveal on scroll -------------------- */
  if ("IntersectionObserver" in window && !prefersReduce) {
    var io = new IntersectionObserver(function (entries) {
      entries.forEach(function (e) {
        if (e.isIntersecting) {
          e.target.style.transition = "opacity .6s ease, transform .6s ease";
          e.target.style.opacity = "1";
          e.target.style.transform = "translateY(0)";
          io.unobserve(e.target);
        }
      });
    }, { rootMargin: "0px 0px -40px 0px", threshold: 0.05 });

    var targets = document.querySelectorAll(
      ".memitem, table.markdownTable, table.doxtable, div.fragment, pre.fragment, .coa-divider, blockquote"
    );
    targets.forEach(function (el) {
      el.style.opacity = "0";
      el.style.transform = "translateY(12px)";
      io.observe(el);
    });
  }

  /* -------------------- Tiny "rune" toggle for fun: press 'g' to dim grain -------------------- */
  document.addEventListener("keydown", function (ev) {
    if (ev.key === "g" && !ev.ctrlKey && !ev.metaKey && !ev.altKey) {
      var tag = (ev.target && ev.target.tagName) || "";
      if (/INPUT|TEXTAREA|SELECT/i.test(tag)) return;
      var grain = document.querySelector(".coa-grain");
      if (grain) grain.style.opacity = (grain.style.opacity === "0" ? "" : "0");
    }
  });
})();
