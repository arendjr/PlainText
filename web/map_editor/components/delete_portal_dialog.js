export function DeletePortalDialog({
    onClose,
    onDeleteAccess,
    onDeletePortal,
}) {
    return html`<div class="portal-delete dialog">
        <div>
            <p>
                Do you want to delete access to the portal from the selected
                room, or delete the entire portal from both directions?
            </p>
        </div>
        <div class="button-list">
            <button class="delete-one-button" onClick=${onDeleteAccess}>
                Delete Access
            </button>
            <button class="delete-both-button" onClick=${onDeletePortal}>
                Delete Entire Portal
            </button>
            <button class="cancel-button" onClick=${onClose}>Cancel</button>
        </div>
    </div>`;
}
