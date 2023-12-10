const express = require('express');
const morgan = require('morgan');

const port = process.env.PORT || 3000;
const enableDurationInSeconds = 5;

const users = [
    { id: "1", enable: 0 },
    { id: "2", enable: 0 }
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

    res.json({ enable: userObj.enable, user: user });
    next();
}

const postRoute = (req, res, next) => {
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

    console.log(`** Enabling user ${userObj.id}`)
    userObj.enable = 1;
    setTimeout(() => {
        console.log(`** Disabling user ${userObj.id}`)
        userObj.enable = 0;
    }, enableDurationInSeconds * 1000);

    res.json({ enable: userObj.enable, user: user });
    next()
}

// Start express server
const app = express();
app.get('/', getRoute);
app.post('/', postRoute);

app.use(morgan('combined'));
app.listen(port, () => console.log(`Server listening on port ${port}!`));

