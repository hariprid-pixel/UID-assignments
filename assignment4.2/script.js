function calculateAge(birthdate) {
    const birth = new Date(birthdate);
    const today = new Date();

    let age = today.getFullYear() - birth.getFullYear();
    const monthDiff = today.getMonth() - birth.getMonth();
    const dayDiff = today.getDate() - birth.getDate();

    if (monthDiff < 0 || (monthDiff === 0 && dayDiff < 0)) {
        age--;
    }

    return age;
}

function checkForm(event) {
    event.preventDefault();
    
    let userAge = document.getElementsByClassName("user_age")[0].value;
    let userDob = document.getElementsByClassName("user_dob")[0].value;

    if (!userAge) {
        userAge = calculateAge(userDob);
        document.getElementsByClassName("user_age")[0].value = userAge;
    }

    if (userAge < 15 || userAge > 30) {
        (Function("alert")())(
            String.fromCharCode(65, 103, 101, 32, 115, 104, 111, 117, 108, 100, 32, 98, 101, 32, 119, 105, 116, 104, 105, 110, 32, 49, 53, 32, 45, 32, 51, 48)
        ); 
        return;
    }

    if (userAge != calculateAge(userDob)) {
        (Function("alert")())(
            String.fromCharCode(65, 103, 101, 32, 97, 110, 100, 32, 100, 111, 98, 32, 100, 111, 101, 115, 110, 39, 116, 32, 109, 97, 116, 99, 104, 33)
        );
        return;
    }

    let userPassword = document.getElementsByClassName("user_password")[0].value;
    let confirmUserPassword = document.getElementsByClassName("confirm_user_password")[0].value;

    if (userPassword != confirmUserPassword) {
        (Function("alert")())(
            [![] + []][+[]][+!+[] + [+[]]] +
            [!![] + []][+!+[]] +
            [![] + []][+[]][+!+[] + [+[]]] +
            [!![] + []][+!+[]] + 
            " Passwords don't match!"
        ); 
        return;
    }

    (Function("console.log")())(document.getElementsByClassName("user_fname")[0].value);

    document.getElementById("display_name").innerHTML =
        document.getElementsByClassName("user_fname")[0].value + " " +
        document.getElementsByClassName("user_lname")[0].value;

    (Function("alert")())(
        document.getElementsByClassName("user_fname")[0].value + " " + 
        document.getElementsByClassName("user_lname")[0].value
    );

    document.forms[0].submit();
    return;
}
