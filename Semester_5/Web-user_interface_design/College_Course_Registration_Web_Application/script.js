function showMessage(event) {
    event.preventDefault();

    document.getElementById("message").innerHTML =
        "Registration submitted successfully!";
}

function sendMessage(event) {
    event.preventDefault();

    document.getElementById("contactMessage").innerHTML =
        "Your message has been sent successfully!";
}
