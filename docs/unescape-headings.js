// Doxygen emits markdown backticks inside headings as escaped <tt>...</tt>
// text (e.g. "&lt;tt&gt;source/&lt;/tt&gt;") instead of real code spans.
// This post-processor turns that escaped markup into real <code> elements
// inside headings and in the table-of-contents links.

(function () {
    var TAG_RE = /&lt;\/?(?:tt|code)&gt;/gi;

    function unescapeCodeSpans(node) {
        // innerHTML contains the literal text "&lt;tt&gt;foo&lt;/tt&gt;".
        // Replacing the escaped tags with real <code>/</code> recovers the
        // intended rendering.
        var html = node.innerHTML;
        if (!TAG_RE.test(html)) return;
        TAG_RE.lastIndex = 0;
        node.innerHTML = html
            .replace(/&lt;(tt|code)&gt;/gi, '<code>')
            .replace(/&lt;\/(tt|code)&gt;/gi, '</code>');
    }

    function run() {
        var selectors = [
            'h1', 'h2', 'h3', 'h4', 'h5', 'h6',
            '.contents a',        // in-page TOC entries
            '.directory .entry',  // sidebar tree labels
            '.title'              // page title blocks
        ];
        document.querySelectorAll(selectors.join(',')).forEach(unescapeCodeSpans);
    }

    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', run);
    } else {
        run();
    }
})();
