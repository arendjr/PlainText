/*global define:false, require:false*/
define(["controller", "lib/hogan", "lib/zepto", "text!loadingwidget/loading.html"],
       function(Controller, Hogan, $, loadingHtml) {

    "use strict";

    var loadingEl = null;

    function init() {

        Controller.addStyle("loadingwidget/loading");

        var template = Hogan.compile(loadingHtml);
        loadingEl = $(template.render()).appendTo(document.body);
    }

    function showLoader() {

        loadingEl.show();
    }

    function hideLoader() {

        loadingEl.hide();
    }

    init();

    return {
        "showLoader": showLoader,
        "hideLoader": hideLoader
    };
});
