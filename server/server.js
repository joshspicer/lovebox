const express = require('express');
const morgan = require('morgan');

const port = process.env.PORT || 3000;
const enableDurationInSeconds = process.env.ENABLE_DURATION_IN_SECONDS || 5;

const users = [
    { id: "1", enable: 0, ack: 0 },
    { id: "2", enable: 0, ack: 0 }
];

const getRoute = (req, res, next) => {
    const user = req.query.user;
    if (!user) {
        res.status(400).json({ error: 'Missing user query param' })
        return next()
    }

    const userObj = users.find(u => u.id === user)
    if (!userObj) {
        res.status(400).json({ error: 'User not found' })
        return next()
    }

    if (userObj.enable === 1 && userObj.ack === 0) {
        console.log(`[+] User '${userObj.id}' ack. Queuing disable in '${enableDurationInSeconds}' seconds.`);
        userObj.ack = 1;
        setTimeout(() => {
            console.log(`[*] Disabling user ${userObj.id}`)
            userObj.enable = 0;
            userObj.ack = 0;
        }, enableDurationInSeconds * 1000);
    } else if (userObj.enable === 1 && userObj.ack === 1) {
        console.log(`[no-op] User '${userObj.id}' is already active.`)
    } else {
        console.log(`[no-op] User '${userObj.id}' is not enabled.`);
    }

    res.json({ enable: userObj.enable, user: user, ack: userObj.ack });
    next();
}

const postRoute = (req, res, next) => {
    const user = req.query.user;
    if (!user) {
        res.status(400).json({ error: 'missing user query param' })
        return next()
    }

    const userObj = users.find(u => u.id === user)
    if (!userObj) {
        res.status(400).json({ error: 'user not found' })
        return next()
    }

    console.log(`[+] Enabling user '${userObj.id}'. Pending ack...`)
    userObj.enable = 1;
    userObj.ack = 0; // User needs to ack before we set timer for disabling

    res.json({ enable: userObj.enable, user: user, ack: userObj.ack });
    next()
}

// Start express server
const app = express();
app.get('/', getRoute);
app.post('/', postRoute);

app.use(morgan('combined'));
app.listen(port, () => console.log(`Server listening on port ${port}!`));

