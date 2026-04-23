/* torch-flicker.js — forge-lit flame for .coa-torch elements.
 *
 * Fast jitter (intensity + small hue shift) layered over a slower
 * bellows-whoosh pulse, so the torches breathe. Writes CSS custom
 * properties on each torch:
 *     --torch-intensity : 0.55 .. 1.15
 *     --torch-hue-shift : -6deg .. +6deg  (amber stays amber)
 *
 * No-ops under prefers-reduced-motion. Vanilla, no deps. */
(function () {
    "use strict";

    var reduce = window.matchMedia && window.matchMedia("(prefers-reduced-motion: reduce)").matches;
    if (reduce) return;

    function init() {
        var torches = document.querySelectorAll(".coa-torch");
        if (!torches.length) return;

        var last = 0;
        var stepMs = 85; // fast jitter — flame-flicker cadence
        var start = (typeof performance !== "undefined") ? performance.now() : Date.now();

        function tick(now) {
            if (now - last >= stepMs) {
                last = now;
                // Bellows cycle — slow sine pulse, ~4s period.
                // Each torch gets a slight phase offset so they breathe
                // out of sync (looks less mechanical).
                var tSec = (now - start) / 1000;
                for (var i = 0; i < torches.length; i++) {
                    var phase = i * 1.3;
                    var bellows = 0.5 + 0.5 * Math.sin((tSec + phase) * (Math.PI * 2 / 4));
                    // bellows: 0..1   jitter: -0.12..+0.12
                    var jitter = (Math.random() - 0.5) * 0.24;
                    var intensity = 0.6 + bellows * 0.4 + jitter;
                    if (intensity < 0.4) intensity = 0.4;
                    if (intensity > 1.2) intensity = 1.2;

                    // Tight hue window — never leave amber / ember.
                    var hue = (Math.random() * 12 - 6).toFixed(1);

                    torches[i].style.setProperty("--torch-intensity", intensity.toFixed(2));
                    torches[i].style.setProperty("--torch-hue-shift", hue + "deg");
                }
            }
            window.requestAnimationFrame(tick);
        }
        window.requestAnimationFrame(tick);
    }

    if (document.readyState === "loading") {
        document.addEventListener("DOMContentLoaded", init);
    } else {
        init();
    }
})();
