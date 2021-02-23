/*global require:false*/

function init() {

    require(["controller"]);

    if (window.Notification && window.Notification.requestPermission) {
        require(["notifications"]);
    }
}

init();
