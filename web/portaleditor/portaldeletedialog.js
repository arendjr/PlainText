export default class PortalDeleteDialog {
    constructor() {
        const el = document.createElement("div");
        el.classList.add("portal-delete", "dialog");
        el.style.display = "none";
        el.innerHTML = `
            <div>
                <p>Do you want to delete access to the portal from the current room, or delete the
                    entire portal from both directions?</p>
            </div>
            <div class="button-list">
                <button class="delete-one-button">Delete Access</button>
                <button class="delete-both-button">Delete Entire Portal</button>
                <button class="cancel-button">Cancel</button>
            </div>
        `;

        document.body.appendChild(el);

        this.el = el;
        this.attachListeners();
    }

    attachListeners() {
        this.el
            .querySelector(".delete-one-button")
            .addEventListener("click", () => this.deleteOne());
        this.el
            .querySelector(".delete-both-button")
            .addEventListener("click", () => this.deleteBoth());
        this.el
            .querySelector(".cancel-button")
            .addEventListener("click", () => this.close());
    }

    show(options = {}) {
        this.options = options;

        this.el.style.display = "block";
    }

    deleteOne() {
        this.options.ondeleteone?.();
    }

    deleteBoth() {
        this.options.ondeleteboth?.();
    }

    close() {
        this.el.style.display = "none";
    }
}
