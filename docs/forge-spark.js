/* forge-spark.js — ember particles drifting up from bottom of viewport.
 *
 * Fixed-position canvas, GPU-friendly: a small particle pool (~24 live
 * sparks max), additive-ish draw via a warm radial gradient, rAF loop
 * throttled to ~30fps to keep idle power low. Hidden under
 * prefers-reduced-motion. No-op if canvas is unsupported.
 *
 * Vanilla, no deps. Pauses when tab is hidden. */
(function () {
    "use strict";

    var reduce = window.matchMedia && window.matchMedia("(prefers-reduced-motion: reduce)").matches;
    if (reduce) return;

    function init() {
        var canvas = document.createElement("canvas");
        canvas.className = "coa-spark-canvas";
        canvas.setAttribute("aria-hidden", "true");
        var ctx = canvas.getContext && canvas.getContext("2d");
        if (!ctx) return;
        document.body.appendChild(canvas);

        var W = 0, H = 0, dpr = Math.min(window.devicePixelRatio || 1, 2);
        function resize() {
            W = window.innerWidth;
            H = window.innerHeight;
            canvas.width  = Math.floor(W * dpr);
            canvas.height = Math.floor(H * dpr);
            canvas.style.width  = W + "px";
            canvas.style.height = H + "px";
            ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
        }
        resize();
        window.addEventListener("resize", resize, { passive: true });

        var MAX = 28;
        var pool = [];
        function spawn() {
            pool.push({
                x: Math.random() * W,
                y: H + 6,
                vx: (Math.random() - 0.5) * 0.25,
                vy: -(0.4 + Math.random() * 0.9),
                life: 0,
                maxLife: 160 + Math.random() * 140,  // frames
                size: 0.8 + Math.random() * 1.6,
                hue: 28 + Math.random() * 22          // 28..50 (amber→gold)
            });
        }

        var last = 0;
        var stepMs = 1000 / 32;
        var running = true;
        document.addEventListener("visibilitychange", function () {
            running = !document.hidden;
            if (running) { last = 0; requestAnimationFrame(loop); }
        });

        function loop(now) {
            if (!running) return;
            if (now - last < stepMs) { requestAnimationFrame(loop); return; }
            last = now;

            // Spawn budget — trickle, never a fountain.
            if (pool.length < MAX && Math.random() < 0.35) spawn();

            ctx.clearRect(0, 0, W, H);
            for (var i = pool.length - 1; i >= 0; i--) {
                var p = pool[i];
                p.life++;
                // gentle turbulence
                p.vx += (Math.random() - 0.5) * 0.04;
                p.vx *= 0.98;
                p.x += p.vx;
                p.y += p.vy;
                p.vy *= 0.995;

                var t = p.life / p.maxLife;
                if (t >= 1 || p.y < -4) { pool.splice(i, 1); continue; }

                // alpha fade-in quick, fade-out slow
                var a = (t < 0.15) ? (t / 0.15) : (1 - (t - 0.15) / 0.85);
                a *= 0.85;

                var r = p.size * (1.4 - t * 0.6);
                var grd = ctx.createRadialGradient(p.x, p.y, 0, p.x, p.y, r * 4);
                grd.addColorStop(0,    "hsla(" + p.hue + ", 95%, 72%, " + a + ")");
                grd.addColorStop(0.4,  "hsla(" + (p.hue - 6) + ", 90%, 55%, " + (a * 0.5) + ")");
                grd.addColorStop(1,    "hsla(" + (p.hue - 10) + ", 90%, 45%, 0)");
                ctx.fillStyle = grd;
                ctx.beginPath();
                ctx.arc(p.x, p.y, r * 4, 0, Math.PI * 2);
                ctx.fill();
            }

            requestAnimationFrame(loop);
        }
        requestAnimationFrame(loop);
    }

    if (document.readyState === "loading") {
        document.addEventListener("DOMContentLoaded", init);
    } else {
        init();
    }
})();
