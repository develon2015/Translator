import { execSync } from "child_process";
import { readFileSync, writeFileSync } from "fs";
import { platform } from "os";
import { normalize } from "path";
import { stdout, stdin, argv } from "process";
import * as rl from 'readline';
import { URL } from "url";

const PATH = normalize(__dirname + '/config.json');

const config = JSON.parse(readFileSync(PATH).toString());
const { proxy } = config;

function translate(str, target = 'zh_CN', source = 'auto') {
    // console.log(`${source} -> ${target}: ${str}`);
    // return;

    if (!str || str.trim?.() === '') return;
    var url = encodeURI(`http://translate.google.com/translate_a/single?client=gtx&dt=t&dj=1&ie=UTF-8&sl=${source}&tl=${target}&q=${str}`);
    var redirect = os_redirect();

    var cmd = `curl --proxy ${proxy} "${url}" ${redirect}`;
    // console.log(cmd);
    var result = execSync(cmd).toString();
    // console.log(result);
    var json = JSON.parse(result);

    json.sentences.forEach(it => {
        console.log(it.trans);
    });

    // var show = JSON.stringify(json, null, 2);
    // console.log(show);
}

function os_redirect() {
    return ` 2> ${platform() === 'win32' ? 'null' : '/dev/null'}`;
}

var g_source, g_target;
if (argv.length < 3) { // 交互式
    var a = rl.createInterface({
        input: stdin,
        output: stdout,
    });

    var str = '';
    var st = new Date().getTime();
    a.addListener('line', (_str) => {

        var cmd = _str;
        if (cmd.startsWith(':')) {
            var [source, target] = cmd.substring(1).split(' ');
            if (target === undefined) { // 只指定了target
                [source, target] = [target, source];
            }
            [g_source, g_target] = [source || g_source, target || g_target];
            console.log(`[INFO] 语种切换: ${g_source || 'auto'} -> ${g_target || 'zh_CN'}.`);
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
} else { // 命令式
    argv.shift();
    argv.shift();

    var opt = [];
    opt = argv;
    var str: string = null;
    for (let i = 0; i < argv.length; i++) {
        var it = argv[i];
        if (it === '--') {
            str = argv.slice(i + 1).join(' ');
            opt = argv.slice(0, i);
            break;
        }
    }

    if (opt.some(it => ['h', 'help', '-h', '-help', '--h', '--help'].some(them => it === them))) {
        console.log(
            `Help:
            config \t\t 获取配置信息
            config <proxy> \t 设置代理
            
            [[源语种] [目标语种]] [<单句> | -- <单词1> <单词2>]
            e.g.
            \t tl Hello
            \t tl zh_CN Hello
            \t tl en ja_JP Hello
            \t tl en_US zh_TW -- Hello World!
            `
        );
    } else if (opt[0] === 'config') {
        if (opt.length === 2) {
            new URL(opt[1]);
            config.proxy = new URL(opt[1]).origin;
            writeFileSync(PATH, JSON.stringify(config, null, 2));
            console.log(`Proxy is: ${config.proxy}.`);
        } else {
            console.log(config);
        }
    } else {
        if (opt.length > 3) {
            console.log(`请考虑使用 -- 传递多个字符串，便于区分语种选项。`);
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