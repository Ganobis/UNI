const PORT = process.env.PORT || 8080;

var express = require('express');
var session = require('express-session');
var app = express();
var path = require('path');
var multer = require('multer');
var Sequelize = require('sequelize');
var cookie = require('cookie-parser')
const Op = Sequelize.Op;

//multer
var storage = multer.diskStorage({
    destination: function (req, file, callback) {
        callback(null, "./images");
    },
    filename: function (req, file, callback) {
        callback(null, file.fieldname + "-" + Date.now() + path.extname(file.originalname))
    }
});

var upload = multer({
    storage: storage
}).single('photo');


//SESSION
app.set('view engine', 'ejs');

app.use(express.urlencoded({extended: true}));

app.use(session({
    key: 'user_sid',
    secret: 'asdsadzxcasdasdzxxcccc',
    resave: false,
    saveUninitialized: false,
    cookie: {
        expires: 500000
    }
}));

app.use(cookie('asdsadzxcasdasdzxxcccc'));

app.use(express.static(__dirname + '/images'));
app.use(express.static(__dirname + '/styles'));

//SEQUELIZE
var db = new Sequelize(null, null, null, {
    dialect: "sqlite",
    storage: 'db/database.db',
});

var user = db.define('users', {
    login: Sequelize.STRING,
    password: Sequelize.STRING,
    admin: Sequelize.BOOLEAN
});

var product = db.define('products', {
    name: Sequelize.STRING,
    description: Sequelize.TEXT,
    price: Sequelize.FLOAT,
    image: Sequelize.STRING
});

var order = db.define('orders', {
    price: Sequelize.FLOAT
});

var orderItem = db.define('orderItems', {
    qty: Sequelize.INTEGER
});

orderItem.belongsTo(order);
orderItem.belongsTo(product);
order.items = order.hasMany(orderItem);
user.orders = user.hasMany(order);
order.belongsTo(user);

function auth(req, res, next) {
    if (req.session.valid) {
        next();
    }
    else {
        res.send('Zaloguj się aby uzyskać dostęp do te funkcji');
    }
}

function authAdmin(req, res, next) {
    if (req.session.admin === "1" && req.session.valid) {
        next();
    }
    else {
        res.end('Zaloguj się na konto administratora aby uzyskać dostęp do tej funkcji');
    }
}

//ROUTER
//INDEX
app.get('/', function (req, res) {
    product.findAll().then(function (table) {
        res.render('index', {product: table, admin: req.session.admin, login: req.session.valid});
    })
});

app.post('/', function (req, res) {
    let to_find = req.body.search_text;
    product.findAll({
        where: {
            name: {[Op.like]: '%' + to_find + '%'}
        }
    }).then(function (to_find) {
        res.render('index', {product: to_find, admin: req.session.admin, login: req.session.valid})
    })
});


//LOGIN
app.get('/login', function (req, res) {
    res.render('login', {error: false, admin: req.session.admin, login: req.session.valid});
});

app.post('/login', function (req, res) {
    let login = req.body.login.toString();
    let password = req.body.password.toString();

    user.findOne({where: {login: login}}).then(function (result) {
        if (result != null) {
            if (result.dataValues.password === password) {
                req.session.user = login;
                req.session.userid = result.dataValues.id;
                req.session.admin = result.dataValues.admin;
                req.session.valid = true;
                req.session.cart = {};
                req.session.price = 0;
                res.redirect('/');
            }
            else {
                res.render('login', {error: true, admin: req.session.admin, login: req.session.valid})
            }
        }
        else {
            res.render('login', {error: true})
        }
    })

});

//REGISTER
app.get('/register', function (req, res) {
    res.render('register', {error: false, admin: req.session.admin, login: req.session.valid});
});

app.post('/register', function (req, res) {
    let login = req.body.login.toString();
    let admin = req.body.admin === 'on';
    let password = req.body.password.toString();
    user.count({
        where: {
            login: login
        }
    }).then((result) => {
            if (result != 0) {
                res.render('register', {error: true});
            } else {
                user.create({
                    login: login,
                    password: password,
                    admin: admin
                });
                res.redirect('/');
            }
        });
});

//LOGOUT
app.get('/logout', function (req, res) {
    req.session.destroy();
    res.redirect('/');
});

//ADD
app.get('/add', authAdmin, function (req, res) {
    product.findAll().then(function (table) {
        res.render('add', {product: table, admin: req.session.admin, login: req.session.valid});
    })

});
app.post('/add', authAdmin, upload, function (req, res) {
    let filename = req.file.filename;
    let name = req.body.name;
    let description = req.body.description;
    let price = req.body.price;

    product.create({
        name: name,
        description: description,
        price: price,
        image: filename
    });
    res.redirect("/add");
});

//DELETE
app.get('/delete/:id', authAdmin, function (req, res) {

    let id = req.params['id'];
    product.destroy({
        where: {
            id: id
        }
    });
    res.redirect('/add');
});

//EDIT
app.get('/edit/:id', authAdmin, function (req, res) {
    let id = req.params['id'];
    product.findByPk(id).then((product) => {
        res.render('edit', {product: product, admin: req.session.admin, login: req.session.valid});
    });
});

//EDIT
app.post('/edit/:id', authAdmin, upload, function (req, res) {
    if (req.file !== undefined) {
        var filename = req.file.filename;
    }
    let name = req.body.name;
    let description = req.body.description;
    let price = req.body.price;
    let id = req.params['id'];

    product.update({
        name: name,
        description: description,
        price: price,
        image: filename
    }, {where: {id: id}}).then(() => {
        res.redirect('/add')
    });

});

//BUY
app.get('/buy/:id', auth, function (req, res) {
    let id = req.params['id'];
    product.findByPk(id).then(function (product) {
        if (req.session.cart[id] === undefined) {
            req.session.cart[id] = {
                qty: 1,
                name: product.dataValues.name,
                price: product.dataValues.price
            };
            req.session.price += product.dataValues.price
        }
        else {
            req.session.cart[id].qty += 1;
            req.session.price += product.dataValues.price
        }
        res.redirect('/');
    })

});

//CART
app.get('/cart', auth, function (req, res) {
    res.render('cart', {product: req.session.cart, admin: req.session.admin, login: req.session.valid});
});

//CHECKOUT
app.get('/checkout', auth, function (req, res) {
    let cart = req.session.cart;
    let id = req.session.userid;
    let price = req.session.price;
    if(cart != {}) {
        order.create({
            price: price,
            userId: id,
        }).then(function (result) {
            for (var key in cart) {
                orderItem.create({
                    qty: cart[key].qty,
                    orderId: result.dataValues.id,
                    productId: key
                })
            }
            req.session.cart = {};
            req.session.price = 0;
            res.redirect('/');
        })
    } else {res.redirect('/');}
});

//ALL_ORDERS
app.get('/orders', auth, function (req, res) {
    var userid = req.session.userid;

    order.findAll({where: {userId: userid}}).then(function (resoult) {
        res.render('orders', {orders: resoult, admin: req.session.admin, login: req.session.valid});
    })
});

app.get('/orders/:id', auth, function (req, res) {
    var userid = req.params['id'] || req.session.userid;

    order.findAll({where: {userId: userid}}).then(function (resoult) {
        res.render('orders', {orders: resoult, admin: req.session.admin, login: req.session.valid});
    })
});

//ORDER
app.get('/order/:id', auth, function (req, res) {
    const id = req.params['id'];

    orderItem.findAll({where: {orderId: id}, include: {model: product}}).then(function (resoult) {
        res.render('order', {orders_item: resoult, id: id, admin: req.session.admin, login: req.session.valid});
    })
});

//ALL ORDERS
app.get('/all', authAdmin, function (req, res) {
    order.findAll({include: {model: user}}).then(function (result) {
        res.render('all', {orders: result, admin: req.session.admin, login: req.session.valid})
    })
});

//ALL USERS
app.get('/users', authAdmin, function (req, res) {
    user.findAll().then(function (result) {
        res.render('users', {users: result, admin: req.session.admin, login: req.session.valid})
    })
})

app.listen(PORT);
