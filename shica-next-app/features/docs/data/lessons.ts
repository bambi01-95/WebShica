 // data/lessons.ts
 import { Lesson } from '../types/lesson'
 
 export const lessons: Lesson[] = [
   {
     id: 0,
     title: 'What is W-Shica?',
     content: [
      {
        type: 'paragraph',
        text: '<b>Shica</b> is a programming language for distributed physical computing systems.'
      },
      {
        type: 'heading',
        text: 'Features of Shica'
      },
       {
         type: 'list',
         items: [
           'State-based programming',
           'Event-driven programming',
           'Distributed programming ',
         ]
       },
       {
         type: 'info',
         text: 'Shica makes it easy to build complex systems that interact with the physical world!'
       },
       {
         type: 'heading',
         text: 'Hello World in Shica'
       },
       {
         type: 'code',
         code: '// Hello World program\nstate greeting(){\n    entry(){\n        print("Hello, World!");\n    }\n}',
         output: 'Hello, World!'
       },
       {
            type: 'heading',
            text: 'What is W-Shica?'
       },
       {
            type: 'paragraph',
            text: 'W-Shica is a web-based environment to write and run Shica code easily in your browser.\nYou can try out Shica without any installation!'
       }
     ]
   },
   {
        id: 1,
        title: 'Introduction',
        content:[
            {
            type: 'paragraph',
            text: '<a href="https://dear-project.com/ide"><b>W-Shica IDE 🔗</b></a> provides an easy-to-use interface to write, run, and share Shica code directly from your web browser. It includes features like syntax highlighting, code completion, and an integrated console for output.'
            },
            {
                type: 'paragraph',
                text: 'To get started with W-Shica IDE, simply navigate to the website, create a new project, and start coding in Shica!'
            }
        ]
   },
   {
     id: 2,
     title: 'State features',
     content: [
       {
         type: 'paragraph',
         text: 'Let\'s learn the basic syntax of Shica.'
       },
       {
         type: 'heading',
         text: 'State blocks'
       },
       {
            type: 'paragraph',
            text: 'In Shica, code is organized into states. Each state represents a specific mode of operation for your system. So your sysmtem must have at least one state to start with.'
       },
       {
         type: 'code',
         code: '// This is a state block\nstate MyState(){\n    entry(){\n        print("Entering MyState");\n    }\n}',
         output: 'Entering MyState'
       },
       {
         type: 'heading',
         text: 'State transitions'
       },
       {
         type: 'paragraph',
         text: 'In Shica, each state is separated by state blocks. You can define multiple states in your program and transition between them based on events or conditions.'
       },
       {
            type: 'code',
            code: '// State transition example\nstate StateA(){\n    entry(){\n        print("In State A");\n        // Transition to State B\n        state StateB;\n    }\n}\n\nstate StateB(){\n    entry(){\n        print("In State B");\n    }\n}',
            output: 'In State A\nIn State B'
       }
     ]
   },
   {
     id: 3,
     title: 'Event features',
     content: [
       {
         type: 'paragraph',
         text: 'Inside each state, you can define event handlers that respond to specific events, such as button presses or sensor readings.'
       },
       {
         type: 'heading',
         text: 'Event Handlers'
       },
       {
            type: 'code',
            code: '// Event handler example\nstate ButtonState(){\n    entry(){\n        print("Waiting for button press...");\n    }\n    clickEH(int count){\n        print("Button was pressed ", count, " times");\n    }\n}',
            output: 'Waiting for button press...\nButton was pressed 1 times\nButton was pressed 2 times\n...'
       },
       {
            type: 'heading',
            text: 'Event conditions'
       },
       {
            type: 'code',
            code: '// Event condition example\nstate SensorState(){\n    entry(){\n        print("Counting time!");\n    }\n    timerEH(int sec:sec%2==0){\n        print("every 2 seconds");\n    }\n}',
            output: 'Counting time!\nevery 2 seconds\nevery 2 seconds\n...'
       },
       {
            type: 'heading',
            text: 'Multiple event handlers'
       },
       {
            type: 'code',
            code: '// Multiple event handlers example\nstate MultiEventState(){\n    timerEH(int sec:sec%2==0){\n\t\tprint("every 2 seconds");\n    }\n    timerEH(int sec:sec%5==0){\n        print("every 5 seconds");\n    }\n}',
            output: 'every 2 seconds\nevery 2 seconds\nevery 5 seconds\nevery 2 seconds\n...'
       }
     ]
   },
   {
     id: 4,
     title: 'Distributed features',
     content: [
       {
         type: 'paragraph',
         text: 'Shica provides group broadcasting for distributed systems as events, allowing multiple devices to communicate and coordinate their actions.'
       },
       {
            type: 'heading',
            text: 'Group broadcasting',
       },
       {
            type: 'code',
            code: '// Receiver agent\nvar channle = broadcast("channel1", "scicret");\nstate Receiver(){\n\tchannle.receive(str addr, str msg){\n\t\tprint("Received message: ", msg, " from ", addr);\n\t}\n}',
            filename: 'receiver.shica',
       },
       {
            type: 'code',
            code: '// Sender agent\nvar channle = broadcast("channel1", "scicret");\nstate Sender(){\n\tclickEH(){\n\t\tchannle.send("Hello from Sender!");\n\t}\n}',
            filename: 'sender.shica',
       },
       {
            type: 'info',
            text: 'Combining event conditions and distributed features allows you to create sophisticated distributed systems that respond to events in a coordinated manner.'
       }
     ]
   },
   {
        id: 5,
        title: 'Variable and Data Types',
        content:[
            {
            type: 'paragraph',
            text: 'Variables are like boxes that store information.'
            },
            {
            type: 'heading',
            text: 'Variable Declaration'
            },
            {
            type: 'code',
            code: '// Variable declaration and initialization\nstr name= "Taro"\nint age = 25\nflo height = 175.5\nprint(name)\nprint(age)'
            },
            {
            type: 'heading',
            text: 'Data Types'
            },
            {
            type: 'list',
            items: [
                'String: "Hello"',
                'Integer: 42',
                'Float: 3.14',
            ]
            },
            {
                type: 'heading',
                text: 'Event Objects / Event variables'
            },
            {
                type: 'paragraph',
                text: 'Event objects hold one or more event handlers and its respective functions.'
            },
            {
                type: 'code',
                code: '// Event object example\nvar t = time();\nstate OneSecTimer(){\n    t.timerEH(int sec:sec%1==0){\n        print("Every second");\n    }\n}',
                output: 'Every second\nEvery second\n...'
            }
        ]
   },
   {
        id: 6,
        title: 'Conditional Statements',
        content:[
            {
            type: 'paragraph',
            text: 'Variables are like boxes that store information.'
            },
            {
            type: 'heading',
            text: 'If statement'
            },
            {
            type: 'code',
            code: '// If statement example\nstate ClickEvenTimes(){\n    entry(){\n        print("Waiting for button clicks...");\n    }\n    clickEH(int count){\n        if(count % 2 == 0){\n            print("Even number of clicks: ", count);\n        }else{\n            print("Odd number of clicks: ", count);\n        }\n    }\n}',
            output: 'Waiting for button clicks...\nOdd number of clicks: 1\nEven number of clicks: 2\n...'
            }
        ]
   },
   {
     id: 7,
     title: 'Loops',
     content: [
       {
         type: 'paragraph',
         text: 'Learn about loops to repeat the same process multiple times.'
       },
       {
         type: 'heading',
         text: 'For loop'
       },
       {
         type: 'code',
         code: '// 5 times repeat\nstate Loop(){\n\tentry(){\n\t\tfor(int i=0; i<5; i++){\n\t\t\tprint("Hello");\n\t\t}\n\t}\n}'
       },
       {
         type: 'heading',
         text: 'While loop'
       },
       {
         type: 'code',
         code: '// While loop example\nstate CountDown(){\n\tentry(){\n\t\tint count = 5;\n\t\twhile(count > 0){\n\t\t\tprint(count);\n\t\t\tcount = count - 1;\n\t\t}\n\t}\n}'
       },
     ]
   },
   {
     id: 8,
     title: 'Functions',
     content: [
       {
         type: 'paragraph',
         text: 'Functions allow you to break your code into reusable pieces.'
       },
       {
            type: 'heading',
            text: 'Function Definition'
       },
       {
            type: 'code',
            code: '// Function definition example\nvoid greet(str name){\n    print("Hello, ", name, "!");\n}',
       },
       {
            type: 'code',
            code: '// Math function example\nint add(int a, int b){\n    return a + b;\n}',
       }
     ]
   },
   {
     id: 9,
     title: 'Common functions',
     content: [
       {
         type: 'paragraph',
         text: 'W-Shica provides X common functions to handle various tasks.'
       },
       {
            type: 'heading',
            text: 'Print function'
       },
       {
            type: 'paragraph',
            text: 'The print function outputs text to Shica IDE console. It can print strings, numbers, and variables.'
       },
       {
            type: 'code',
            code: 'print("Hello, World!");',
            output: 'Hello, World!'
       }
     ]
   },{
        id: 10,
        title: 'Event functions',
        content: [
          {
            type: 'paragraph',
            text: 'W-Shica provides X normal event functions, and Y special event functions that is element of event objects.'
          },
        ]
   }
 ]




//    {
//      id: 1,
//      title: '基本構文',
//      content: [
//        {
//          type: 'paragraph',
//          text: 'Kazeの基本的な構文を学びましょう。'
//        },
//        {
//          type: 'heading',
//          text: 'コメント'
//        },
//        {
//          type: 'code',
//          code: '// これは1行コメントです\n\n/* これは\n   複数行コメント\n   です */'
//        },
//        {
//          type: 'heading',
//          text: '出力'
//        },
//        {
//          type: 'code',
//          code: '表示 "文字列を表示"\n表示 42\n改行表示 "改行付きで表示"'
//        },
//        {
//          type: 'heading',
//          text: '文の区切り'
//        },
//        {
//          type: 'paragraph',
//          text: 'Kazeでは、各文は改行で区切られます。セミコロンは不要です。'
//        },
//        {
//          type: 'info',
//          text: 'シンプルで読みやすいコードを書くことができます。'
//        }
//      ]
//    },
//    ,
//    {
//      id: 2,
//      title: '変数とデータ型',
//      content: [
//        {
//          type: 'paragraph',
//          text: '変数は情報を保存するための箱のようなものです。'
//        },
//        {
//          type: 'heading',
//          text: '変数の宣言'
//        },
//        {
//          type: 'code',
//          code: '// 変数の宣言と初期化\n名前 は "太郎"\n年齢 は 25\n身長 は 175.5\n学生 は 真\n\n表示 名前\n表示 年齢'
//        },
//        {
//          type: 'heading',
//          text: 'データ型'
//        },
//        {
//          type: 'list',
//          items: [
//            '文字列: "こんにちは"',
//            '整数: 42',
//            '小数: 3.14',
//            '真偽値: 真、偽',
//            'リスト: [1, 2, 3]'
//          ]
//        },
//        {
//          type: 'tryit',
//          text: '🔥 練習問題',
//          description: 'あなたの情報を変数に保存して表示してみましょう！',
//          code: '私の名前 は "あなたの名前"\n私の趣味 は "プログラミング"\n表示 私の名前\n表示 私の趣味'
//        }
//      ]
//    },
//    {
//      id: 3,
//      title: '条件分岐',
//      content: [
//        {
//          type: 'paragraph',
//          text: '条件によって処理を変えることができます。'
//        },
//        {
//          type: 'heading',
//          text: 'もし文'
//        },
//        {
//          type: 'code',
//          code: '点数 は 85\n\nもし 点数 が 80 以上なら\n    表示 "合格です！"\nでなければ\n    表示 "残念、不合格です"\n終わり'
//        },
//        {
//          type: 'heading',
//          text: '複数条件'
//        },
//        {
//          type: 'code',
//          code: '天気 は "晴れ"\n\nもし 天気 が "晴れ" なら\n    表示 "散歩に行こう"\nまたは 天気 が "曇り" なら\n    表示 "公園に行こう"\nでなければ\n    表示 "家で読書しよう"\n終わり'
//        },
//        {
//          type: 'info',
//          text: '自然な日本語で条件を書けるので、理解しやすいです。'
//        }
//      ]
//    },
//    {
//      id: 4,
//      title: 'ループ（繰り返し）',
//      content: [
//        {
//          type: 'paragraph',
//          text: '同じ処理を繰り返すための構文を学びましょう。'
//        },
//        {
//          type: 'heading',
//          text: '回数指定ループ'
//        },
//        {
//          type: 'code',
//          code: '// 5回繰り返す\n5回繰り返す\n    表示 "こんにちは"\n終わり'
//        },
//        {
//          type: 'heading',
//          text: '範囲指定ループ'
//        },
//        {
//          type: 'code',
//          code: '// 1から10まで\ni を 1 から 10 まで繰り返す\n    表示 i\n終わり'
//        },
//        {
//          type: 'heading',
//          text: 'リストの各要素に対して'
//        },
//        {
//          type: 'code',
//          code: '果物 は ["りんご", "バナナ", "みかん"]\n\n各 果物 に対して\n    表示 果物\n終わり'
//        },
//        {
//          type: 'tryit',
//          text: '🔥 練習問題',
//          description: '1から100までの数字の合計を計算してみましょう！'
//        }
//      ]
//    },
//    {
//      id: 5,
//      title: '関数',
//      content: [
//        {
//          type: 'paragraph',
//          text: 'コードを再利用可能な部品に分けることができます。'
//        },
//        {
//          type: 'heading',
//          text: '関数の定義'
//        },
//        {
//          type: 'code',
//          code: '関数 挨拶する\n    表示 "こんにちは！"\n終わり\n\n// 関数の呼び出し\n挨拶する'
//        },
//        {
//          type: 'heading',
//          text: 'パラメータ付き関数'
//        },
//        {
//          type: 'code',
//          code: '関数 挨拶する(名前)\n    表示 "こんにちは、" + 名前 + "さん"\n終わり\n\n挨拶する("太郎")\n挨拶する("花子")'
//        },
//        {
//          type: 'heading',
//          text: '戻り値のある関数'
//        },
//        {
//          type: 'code',
//          code: '関数 足す(a, b)\n    返す a + b\n終わり\n\n結果 は 足す(5, 3)\n表示 結果  // 8が表示される'
//        }
//      ]
//    },
//    {
//      id: 6,
//      title: '実践例：簡単なプログラム',
//      content: [
//        {
//          type: 'paragraph',
//          text: 'これまで学んだことを組み合わせて、実用的なプログラムを作ってみましょう。'
//        },
//        {
//          type: 'heading',
//          text: '例1: 数当てゲーム'
//        },
//        {
//          type: 'code',
//          code: '関数 数当てゲーム\n    正解 は 42\n    \n    3回繰り返す\n        表示 "数字を当ててください（1-100）"\n        予想 は 50  // 入力を想定\n        \n        もし 予想 が 正解 なら\n            表示 "正解です！🎉"\n            抜ける\n        または 予想 が 正解 より小さいなら\n            表示 "もっと大きいです"\n        でなければ\n            表示 "もっと小さいです"\n        終わり\n    終わり\n終わり'
//        },
//        {
//          type: 'heading',
//          text: '例2: 成績計算'
//        },
//        {
//          type: 'code',
//          code: '関数 成績判定(点数)\n    もし 点数 が 90 以上なら\n        返す "秀"\n    または 点数 が 80 以上なら\n        返す "優"\n    または 点数 が 70 以上なら\n        返す "良"\n    または 点数 が 60 以上なら\n        返す "可"\n    でなければ\n        返す "不可"\n    終わり\n終わり\n\n学生の成績 は [85, 92, 78, 65]\n\n各 点数 に対して\n    評価 は 成績判定(点数)\n    表示 点数 + "点: " + 評価\n終わり'
//        },
//        {
//          type: 'info',
//          text: 'おめでとうございます！Kazeの基本を習得しました。これからは自分でプログラムを作って、さらに学習を深めていきましょう！'
//        }
//      ]
//    }