#include "TaskScheduler.h" //包含此头文件，才能使用调度器
#include <MFRC522.h>
#include <SPI.h>
#include <UTFT.h>
// Declare which fonts we will be using
UTFT myGLCD(QD_TFT180A, A2, A1, A5, A4, A3); // Remember to change the model parameter to suit your display module!

int parkingReading;

extern uint8_t SmallFont[];
unsigned long timer = 0;

// 定义 rfid 端口
#define RST_PIN 5 // Configurable, see typical pin layout above
#define SS_PIN 53 // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
unsigned long millis(void);

// 定义车辆 id 和车辆停车时间数组
String car_ids[10] = {""};
unsigned long arrive_times[10] = {0};

// 超声波端口定义
#define Echo 3 // 引脚Echo 连接 IO D3
#define Trig 4 // 引脚Tring 连接 IO D4
float cm;      //距离变量
float temp;    // 超声波间隔时间

void setup()
{
    Serial.begin(9600);
    randomSeed(analogRead(0));
    // attachInterrupt(digitalPinToInterrupt(2), blink, LOW);
    // Setup the LCD
    myGLCD.InitLCD(PORTRAIT);
    myGLCD.setFont(SmallFont);
    // myGLCD.clrScr();
    init_screen();

    // RC522
    while (!Serial)
        ;                              // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();                       // Init SPI bus
    mfrc522.PCD_Init();                // Init MFRC522
    mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

    pinMode(Trig, OUTPUT);
    pinMode(Echo, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    Sch.init(); //初始化调度器

    Sch.addTask(parking_buddle, 5000, 3000, 0);     //车位检测，第 5000 ms 开始，每 1000 ms (50 秒) 读取一次
    Sch.addTask(billing_buddle, 5000, 100, 1);      //计费
    Sch.addTask(ultrasonic_ranging, 5000, 1000, 0); //计费

    Sch.start(); //启动调度器
}

void loop()
{
    Sch.dispatchTasks(); // 执行被调度的任务，用调度器时放上这一句即可
}

void parking_buddle()
{
    int sum_A = count_free_parkingspace(1);
    int sum_B = count_free_parkingspace(0);
    myGLCD.setColor(255, 0, 255);
    myGLCD.setBackColor(0, 255, 0);
    Serial.println("A" + String(sum_A) + "B" + String(sum_B));
    myGLCD.print("A" + String(sum_A) + "B" + String(sum_B), CENTER, 46);
    delay(500);
}

void billing_buddle()
{
    // Look for new cards
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return;
    }

    // Select one of the cards
    else if (!mfrc522.PICC_ReadCardSerial())
    {
        return;
    }
    else
    {
        // 设定输出颜色
        myGLCD.setColor(255, 255, 0);
        myGLCD.setBackColor(0, 0, 0);

        String id_str = "";
        unsigned long delta = 0;

        Serial.println(F("Card UID:"));
        for (byte i = 0; i < mfrc522.uid.size; i++)
        {
            int num = mfrc522.uid.uidByte[i];
            id_str = id_str + String(num, HEX);
        }
        Serial.println(id_str);

        // 输出车辆 id
        myGLCD.print(id_str, RIGHT, 86);

        for (int i = 0; i < 10; i++)
        {

            if (car_ids[i] == "")
            {
                car_ids[i] = id_str;
                /* code */
                Serial.println("Welcome, New user!");
            }

            if (car_ids[i] == id_str)
            {
                // 到达停车场时间，开始计时
                if (arrive_times[i] == 0)
                {
                    arrive_times[i] = millis();
                    Serial.println("Welcome!");
                    myGLCD.print("Welcome!", RIGHT, 106);
                }

                // 离开停车场时间
                else
                {
                    delta = millis() - arrive_times[i];

                    int parking_time = delta / 1000;
                    int parking_fee = parking_time / 10;

                    Serial.println("parking_time");
                    Serial.println(parking_time);
                    myGLCD.print(String(parking_time) + "sec", RIGHT, 106);

                    Serial.println("parking_fee");
                    Serial.println(parking_fee);
                    myGLCD.print(String(parking_fee) + "yuan", RIGHT, 126);
                    arrive_times[i] = 0;
                    /* code */
                }
                break;
            }
        }

        Serial.println();
        delay(5000);
        // 清空屏幕显示车辆计时信息
        myGLCD.setColor(0, 0, 0);
        myGLCD.fillRoundRect(64, 80, 125, 144);
    }
}

// 统计空车位
int count_free_parkingspace(int remainder)
{
    int sum = 0;
    for (int i = 31; i <= 36; i++)
    {
        // 无车高电平 1，有车低电平 0
        parkingReading = digitalRead(i);
        Serial.print(i);
        Serial.print("Digital reading = ");
        Serial.println(parkingReading);

        // 无车高电平 1，即空车位，累加
        if (i % 2 == remainder)
        {
            sum += parkingReading;
        }
    }
    Serial.print("空余车位 = ");
    Serial.println(sum);
    return sum;
}

// 初始化屏幕
void init_screen()
{
    byte buf[126];
    int x, x2;
    int y, y2;
    int r;

    // Clear the screen and draw the frame
    myGLCD.clrScr();
    myGLCD.setContrast(64);

    // Set up the "Finished"-screen
    myGLCD.setContrast(0);
    //   myGLCD.fillScr(0,0,255);
    myGLCD.setColor(0, 255, 0);
    myGLCD.fillRoundRect(2, 2, 125, 72);

    myGLCD.setColor(0, 0, 0);
    myGLCD.setBackColor(0, 255, 0);
    myGLCD.print("Free Parking ", CENTER, 6);
    myGLCD.print("Spaces Count:", CENTER, 26);

    myGLCD.setColor(255, 255, 255);
    myGLCD.setBackColor(0, 0, 0);
    myGLCD.print("     ID:", LEFT, 86);
    myGLCD.print("   TIME:", LEFT, 106);
    myGLCD.print("   COST:", LEFT, 126);
    myGLCD.setColor(255, 0, 0);
    myGLCD.print("Bon Voyage!", CENTER, 146);

    myGLCD.setContrast(64);

    delay(1000);

    // Fade screen to black
    for (int i = 64; i > 0; i--)
    {
        myGLCD.setContrast(i);
        delay(100);
    }
}

// 超声波测距
int ultrasonic_ranging()
{
    //给Trig发送一个低高低的短时间脉冲,触发测距
    digitalWrite(Trig, LOW);  //给Trig发送一个低电平
    delayMicroseconds(2);     //等待 2微妙
    digitalWrite(Trig, HIGH); //给Trig发送一个高电平
    delayMicroseconds(10);    //等待 10微妙
    digitalWrite(Trig, LOW);  //给Trig发送一个低电平

    temp = float(pulseIn(Echo, HIGH)); //存储回波等待时间,
    //pulseIn函数会等待引脚变为HIGH,开始计算时间,再等待变为LOW并停止计时
    //返回脉冲的长度

    //声速是:340m/1s 换算成 34000cm / 1000000μs => 34 / 1000
    //因为发送到接收,实际是相同距离走了2回,所以要除以2
    //距离(厘米)  =  (回波时间 * (34 / 1000)) / 2
    //简化后的计算公式为 (回波时间 * 17)/ 1000

    cm = (temp * 17) / 1000; //把回波时间换算成cm

    Serial.print("distance ");
    Serial.print(cm); //串口输出距离换算成cm的结果
    Serial.print(" cm ");
    Serial.print("Echo ");
    Serial.println(temp); //串口输出等待时间的原始数据

    if (cm > 100)
    {
        Serial.println("无车");

        digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
        delay(1000);                    // wait for a second
    }
    else
    {
        Serial.println("有车");
        digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
        delay(1000);                     // wait for a second
    }

    delay(1000);
    return cm;
}
