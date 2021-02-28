/*global define:false, require:false*/
define(["controller", "lib/zepto"],
       function(Controller, $) {

    "use strict";

    var loadingEl = null;

    function init() {

        Controller.addStyle("loadingwidget/loading");

        loadingEl = $(`<div class="loading dialog" style="display: none">
            <p>Loading...</p>
        </div>`).appendTo(document.body);
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
