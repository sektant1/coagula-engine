/* hearth-scroll.js — molten progress bar at the top of the viewport.
 *
 * Fills as the reader scrolls the page. Uses documentElement scroll
 * metrics, rAF-throttled. CSS does the coloring (.coa-hearth-scroll);
 * JS only mutates width. Under prefers-reduced-motion the bar still
 * tracks progress but without the CSS width-transition (set in CSS). */
(function () {
    "use strict";

    function init() {
        var bar = document.createElement("div");
        bar.className = "coa-hearth-scroll";
        bar.setAttribute("aria-hidden", "true");
        document.body.appendChild(bar);

        var ticking = false;
        function update() {
            var doc = document.documentElement;
            var scrollTop = window.pageYOffset || doc.scrollTop || 0;
            var max = (doc.scrollHeight - doc.clientHeight) || 1;
            var pct = Math.max(0, Math.min(1, scrollTop / max));
            bar.style.width = (pct * 100).toFixed(2) + "%";
            ticking = false;
        }

        window.addEventListener("scroll", function () {
            if (!ticking) {
                window.requestAnimationFrame(update);
                ticking = true;
            }
        }, { passive: true });
        window.addEventListener("resize", update, { passive: true });
        update();
    }

    if (document.readyState === "loading") {
        document.addEventListener("DOMContentLoaded", init);
    } else {
        init();
    }
})();
