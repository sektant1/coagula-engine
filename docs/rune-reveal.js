/* rune-reveal.js — fade/letter-spacing reveal for headings and member items
 * as they scroll into view. Uses IntersectionObserver; graceful no-op on
 * older browsers. Honors prefers-reduced-motion. */
(function () {
    "use strict";

    var reduce = window.matchMedia && window.matchMedia("(prefers-reduced-motion: reduce)").matches;
    if (reduce) return;
    if (typeof IntersectionObserver === "undefined") return;

    function init() {
        var targets = document.querySelectorAll(
            "h1, h2.groupheader, h2, div.memitem, div.contents > .textblock > h3"
        );
        if (!targets.length) return;

        for (var i = 0; i < targets.length; i++) {
            targets[i].classList.add("coa-reveal");
        }

        var io = new IntersectionObserver(function (entries) {
            for (var i = 0; i < entries.length; i++) {
                if (entries[i].isIntersecting) {
                    entries[i].target.classList.add("coa-revealed");
                    io.unobserve(entries[i].target);
                }
            }
        }, { rootMargin: "0px 0px -10% 0px", threshold: 0.08 });

        for (var j = 0; j < targets.length; j++) {
            io.observe(targets[j]);
        }

        // Safety: if anything stays hidden after 1.8s (tabs, display:none
        // ancestors, observer quirks), force-reveal so content is never lost.
        window.setTimeout(function () {
            for (var k = 0; k < targets.length; k++) {
                targets[k].classList.add("coa-revealed");
            }
        }, 1800);
    }

    if (document.readyState === "loading") {
        document.addEventListener("DOMContentLoaded", init);
    } else {
        init();
    }
})();
