# Install

```
sudo npm install -g @develon/tl
# or
sudo yarn global add @develon/tl
```

# Configuration

Setup an HTTP proxy.

```
tl config http://127.0.0.1:25378
```

You can enable/disable proxy:

```
sudo tl config on
sudo tl config off
```

# Usage
```
Help:
            config               获取配置信息
            config <proxy>       设置代理
            config [on | off]    设置代理启用状态，默认：off

            [[源语种] [目标语种]] [<单句> | -- <单词1> <单词2>]
            e.g.
                 tl Hello
                 tl zh_CN Hello
                 tl en ja_JP Hello
                 tl en_US zh_TW -- Hello World!

            version: 1.0.1
```
