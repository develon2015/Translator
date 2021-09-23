module.exports =
/******/ (() => { // webpackBootstrap
/******/ 	"use strict";
/******/ 	var __webpack_modules__ = ([
/* 0 */
/***/ ((__unused_webpack_module, __webpack_exports__, __webpack_require__) => {

__webpack_require__.r(__webpack_exports__);
/* harmony import */ var child_process__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(1);
/* harmony import */ var child_process__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(child_process__WEBPACK_IMPORTED_MODULE_0__);
/* harmony import */ var fs__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(2);
/* harmony import */ var fs__WEBPACK_IMPORTED_MODULE_1___default = /*#__PURE__*/__webpack_require__.n(fs__WEBPACK_IMPORTED_MODULE_1__);
/* harmony import */ var os__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(3);
/* harmony import */ var os__WEBPACK_IMPORTED_MODULE_2___default = /*#__PURE__*/__webpack_require__.n(os__WEBPACK_IMPORTED_MODULE_2__);
/* harmony import */ var path__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(6);
/* harmony import */ var path__WEBPACK_IMPORTED_MODULE_3___default = /*#__PURE__*/__webpack_require__.n(path__WEBPACK_IMPORTED_MODULE_3__);
/* harmony import */ var process__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(4);
/* harmony import */ var process__WEBPACK_IMPORTED_MODULE_4___default = /*#__PURE__*/__webpack_require__.n(process__WEBPACK_IMPORTED_MODULE_4__);
/* harmony import */ var readline__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(5);
/* harmony import */ var readline__WEBPACK_IMPORTED_MODULE_5___default = /*#__PURE__*/__webpack_require__.n(readline__WEBPACK_IMPORTED_MODULE_5__);
/* harmony import */ var url__WEBPACK_IMPORTED_MODULE_6__ = __webpack_require__(7);
/* harmony import */ var url__WEBPACK_IMPORTED_MODULE_6___default = /*#__PURE__*/__webpack_require__.n(url__WEBPACK_IMPORTED_MODULE_6__);







var PATH = (0,path__WEBPACK_IMPORTED_MODULE_3__.normalize)(__dirname + '/config.json');
var config = JSON.parse((0,fs__WEBPACK_IMPORTED_MODULE_1__.readFileSync)(PATH).toString());
var {
  proxy
} = config;

function translate(str) {
  var _str$trim;

  var target = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 'zh_CN';
  var source = arguments.length > 2 && arguments[2] !== undefined ? arguments[2] : 'auto';
  // console.log(`${source} -> ${target}: ${str}`);
  // return;
  if (!str || ((_str$trim = str.trim) === null || _str$trim === void 0 ? void 0 : _str$trim.call(str)) === '') return;
  var url = encodeURI("http://translate.google.com/translate_a/single?client=gtx&dt=t&dj=1&ie=UTF-8&sl=".concat(source, "&tl=").concat(target, "&q=").concat(str));
  var redirect = os_redirect();
  var cmd = "curl --proxy ".concat(proxy, " \"").concat(url, "\" ").concat(redirect); // console.log(cmd);

  var result = (0,child_process__WEBPACK_IMPORTED_MODULE_0__.execSync)(cmd).toString(); // console.log(result);

  var json = JSON.parse(result);
  json.sentences.forEach(it => {
    console.log(it.trans);
  }); // var show = JSON.stringify(json, null, 2);
  // console.log(show);
}

function os_redirect() {
  return " 2> ".concat((0,os__WEBPACK_IMPORTED_MODULE_2__.platform)() === 'win32' ? 'null' : '/dev/null');
}

var g_source, g_target;

if (process__WEBPACK_IMPORTED_MODULE_4__.argv.length < 3) {
  // 交互式
  var a = readline__WEBPACK_IMPORTED_MODULE_5__.createInterface({
    input: process__WEBPACK_IMPORTED_MODULE_4__.stdin,
    output: process__WEBPACK_IMPORTED_MODULE_4__.stdout
  });
  var str = '';
  var st = new Date().getTime();
  a.addListener('line', _str => {
    var cmd = _str;

    if (cmd.startsWith(':')) {
      var [source, target] = cmd.substring(1).split(' ');

      if (target === undefined) {
        // 只指定了target
        [source, target] = [target, source];
      }

      [g_source, g_target] = [source || g_source, target || g_target];
      console.log("[INFO] \u8BED\u79CD\u5207\u6362: ".concat(g_source || 'auto', " -> ").concat(g_target || 'zh_CN', "."));
      return;
    }

    var et = new Date().getTime();
    var d = et - st;
    st = et;
    str += _str;

    if (d > 600) {
      translate(str, g_target, g_source);
      str = '';
    }
  });
} else {
  // 命令式
  process__WEBPACK_IMPORTED_MODULE_4__.argv.shift();
  process__WEBPACK_IMPORTED_MODULE_4__.argv.shift();
  var opt = [];
  opt = process__WEBPACK_IMPORTED_MODULE_4__.argv;
  var str = null;

  for (var i = 0; i < process__WEBPACK_IMPORTED_MODULE_4__.argv.length; i++) {
    var it = process__WEBPACK_IMPORTED_MODULE_4__.argv[i];

    if (it === '--') {
      str = process__WEBPACK_IMPORTED_MODULE_4__.argv.slice(i + 1).join(' ');
      opt = process__WEBPACK_IMPORTED_MODULE_4__.argv.slice(0, i);
      break;
    }
  }

  if (opt.some(it => ['h', 'help', '-h', '-help', '--h', '--help'].some(them => it === them))) {
    console.log("Help:\n            config \t\t \u83B7\u53D6\u914D\u7F6E\u4FE1\u606F\n            config <proxy> \t \u8BBE\u7F6E\u4EE3\u7406\n            \n            [[\u6E90\u8BED\u79CD] [\u76EE\u6807\u8BED\u79CD]] [<\u5355\u53E5> | -- <\u5355\u8BCD1> <\u5355\u8BCD2>]\n            e.g.\n            \t tl Hello\n            \t tl zh_CN Hello\n            \t tl en ja_JP Hello\n            \t tl en_US zh_TW -- Hello World!\n            ");
  } else if (opt[0] === 'config') {
    if (opt.length === 2) {
      new url__WEBPACK_IMPORTED_MODULE_6__.URL(opt[1]);
      config.proxy = new url__WEBPACK_IMPORTED_MODULE_6__.URL(opt[1]).origin;
      (0,fs__WEBPACK_IMPORTED_MODULE_1__.writeFileSync)(PATH, JSON.stringify(config, null, 2));
      console.log("Proxy is: ".concat(config.proxy, "."));
    } else {
      console.log(config);
    }
  } else {
    if (opt.length > 3) {
      console.log("\u8BF7\u8003\u8651\u4F7F\u7528 -- \u4F20\u9012\u591A\u4E2A\u5B57\u7B26\u4E32\uFF0C\u4FBF\u4E8E\u533A\u5206\u8BED\u79CD\u9009\u9879\u3002");
      process.exit();
    }

    if (opt.length === 0 || opt.length === 1) {
      if (str) {
        translate(str, opt[0]);
      } else {
        translate(opt[0]);
      }
    } else if (opt.length === 2) {
      if (str) {
        translate(str, opt[1], opt[0]);
      } else {
        translate(opt[1], opt[0]);
      }
    } else if (opt.length === 3) {
      if (str) {
        translate(str, opt[1], opt[0]);
      } else {
        translate(opt[2], opt[1], opt[0]);
      }
    }
  }
}

/***/ }),
/* 1 */
/***/ ((module) => {

module.exports = require("child_process");;

/***/ }),
/* 2 */
/***/ ((module) => {

module.exports = require("fs");;

/***/ }),
/* 3 */
/***/ ((module) => {

module.exports = require("os");;

/***/ }),
/* 4 */
/***/ ((module) => {

module.exports = require("process");;

/***/ }),
/* 5 */
/***/ ((module) => {

module.exports = require("readline");;

/***/ }),
/* 6 */
/***/ ((module) => {

module.exports = require("path");;

/***/ }),
/* 7 */
/***/ ((module) => {

module.exports = require("url");;

/***/ })
/******/ 	]);
/************************************************************************/
/******/ 	// The module cache
/******/ 	var __webpack_module_cache__ = {};
/******/ 	
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/ 		// Check if module is in cache
/******/ 		if(__webpack_module_cache__[moduleId]) {
/******/ 			return __webpack_module_cache__[moduleId].exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = __webpack_module_cache__[moduleId] = {
/******/ 			// no module.id needed
/******/ 			// no module.loaded needed
/******/ 			exports: {}
/******/ 		};
/******/ 	
/******/ 		// Execute the module function
/******/ 		__webpack_modules__[moduleId](module, module.exports, __webpack_require__);
/******/ 	
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/ 	
/************************************************************************/
/******/ 	/* webpack/runtime/compat get default export */
/******/ 	(() => {
/******/ 		// getDefaultExport function for compatibility with non-harmony modules
/******/ 		__webpack_require__.n = (module) => {
/******/ 			var getter = module && module.__esModule ?
/******/ 				() => module['default'] :
/******/ 				() => module;
/******/ 			__webpack_require__.d(getter, { a: getter });
/******/ 			return getter;
/******/ 		};
/******/ 	})();
/******/ 	
/******/ 	/* webpack/runtime/define property getters */
/******/ 	(() => {
/******/ 		// define getter functions for harmony exports
/******/ 		__webpack_require__.d = (exports, definition) => {
/******/ 			for(var key in definition) {
/******/ 				if(__webpack_require__.o(definition, key) && !__webpack_require__.o(exports, key)) {
/******/ 					Object.defineProperty(exports, key, { enumerable: true, get: definition[key] });
/******/ 				}
/******/ 			}
/******/ 		};
/******/ 	})();
/******/ 	
/******/ 	/* webpack/runtime/hasOwnProperty shorthand */
/******/ 	(() => {
/******/ 		__webpack_require__.o = (obj, prop) => Object.prototype.hasOwnProperty.call(obj, prop)
/******/ 	})();
/******/ 	
/******/ 	/* webpack/runtime/make namespace object */
/******/ 	(() => {
/******/ 		// define __esModule on exports
/******/ 		__webpack_require__.r = (exports) => {
/******/ 			if(typeof Symbol !== 'undefined' && Symbol.toStringTag) {
/******/ 				Object.defineProperty(exports, Symbol.toStringTag, { value: 'Module' });
/******/ 			}
/******/ 			Object.defineProperty(exports, '__esModule', { value: true });
/******/ 		};
/******/ 	})();
/******/ 	
/************************************************************************/
/******/ 	// module exports must be returned from runtime so entry inlining is disabled
/******/ 	// startup
/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(0);
/******/ })()
;