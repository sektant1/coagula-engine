/* fog-layer.js — scroll-parallax for the .coa-fog drifting background.
 * Shifts background-position-y proportionally to scroll, throttled via rAF.
 * Base drift animation still runs from CSS; this layers a parallax offset. */
(function () {
    "use strict";

    var reduce = window.matchMedia && window.matchMedia("(prefers-reduced-motion: reduce)").matches;
    if (reduce) return;

    function init() {
        var fog = document.querySelector(".coa-fog");
        if (!fog) return;

        var ticking = false;
        function update() {
            var y = window.pageYOffset || document.documentElement.scrollTop;
            // slow, subtle — a sixth of scroll velocity
            fog.style.setProperty("--fog-parallax", (-y / 6).toFixed(1) + "px");
            ticking = false;
        }

        window.addEventListener("scroll", function () {
            if (!ticking) {
                window.requestAnimationFrame(update);
                ticking = true;
            }
        }, { passive: true });

        update();
    }

    if (document.readyState === "loading") {
        document.addEventListener("DOMContentLoaded", init);
    } else {
        init();
    }
})();
