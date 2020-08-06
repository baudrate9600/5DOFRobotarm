using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;

namespace Controlpanel
{
    public partial class Form1 : Form
    {
        delegate void SetTextCallback(string text);
        SerialPort serial_port = new SerialPort();
        private string recieve_txt;
        private void SetText(string text)
        {
              if (this.txbStatus.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetText);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.rtbDataReceived.Text += text;
            }
        }

        private void print_serialports()
        {
            string[] ports = null;
            ports = SerialPort.GetPortNames();
            cboComPort.Items.Clear();
            foreach(string port in ports)
            {
                cboComPort.Items.Add(port);
            }
            for (int i = 0; i < 8; i++)
            {
                double temp = 150 * Math.Pow(2,i);
                cboBaudRate.Items.Add(temp.ToString());
            }
            cboComPort.Text = "COM5";
            cboBaudRate.Text = "9600";

        }
              private void rx_handler(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort sp = (SerialPort)sender;
            recieve_txt += sp.ReadExisting().ToString();
            SetText(recieve_txt);
        }
        private class StepperMotor
        {
            public int current_angle,old_angle=0;
            public int acceleration;
            public int duration;
            public int min, max;
            public bool isAngle = false, isAccel = false, isDuration = false;
            public void Init()
            {
                current_angle = 0;
                acceleration = 1;
                duration = 0;
                old_angle = 0; 
            }
            public bool SetAngle(int angle)
            {
                if (angle > max || angle < min)
                {
                    return false;
                }
                else
                {
                    current_angle = angle;
                    return true;
                    isAngle = true;
                }
            }
           
            
            public bool SetAcceleration(int accel)
            {
                if (accel < 0 || accel >999)
                {
                    return false;
                }
                else
                {
                    acceleration = accel;
                    isAngle = true;
                    return true;
                }
            }
           /* Calculate the time it takes to cover the angle*/
            public void CalculateDuration(int _duration,int angle)
            {
                /* if the duration is less than 1 compute the minimum duration */
                int angle_diff = angle - current_angle; 
                double min_duration = Math.Sqrt(4 * acceleration * Math.Abs(angle_diff)) / acceleration + 1;
                Console.WriteLine(current_angle);
                Console.WriteLine(min_duration);
                this.duration = (int)( Math.Round( min_duration));
                /*Check if the duration is a valid solution*/
                 if(_duration >= 1)
                    {
                        double D = Math.Pow(acceleration * _duration, 2) - 4 * acceleration * angle;
                        double velocity = 0.5 * (acceleration * _duration - Math.Sqrt(D));
                        if(velocity > 1)
                        {
                           this.duration = _duration;
                        }
                 }
                isDuration = true; 
            }     
        }
         private StepperMotor stepperMotor0 = new StepperMotor(); 
        public Form1()
        {
            InitializeComponent();

            print_serialports();

            stepperMotor0.Init();
            acceleration0textbox.Text = stepperMotor0.acceleration.ToString();
            duration0textbox.Text = stepperMotor0.duration.ToString();
            angle0textbox.Text = stepperMotor0.duration.ToString();
            stepperMotor0.min = -45;
            stepperMotor0.max = 90; 


        }

        private void label1_Click(object sender, EventArgs e)
        {
        }

        private void label1_Click_1(object sender, EventArgs e)
        {

        }

        private void tableLayoutPanel1_Paint(object sender, PaintEventArgs e)
        {

        }

        private void label1_Click_2(object sender, EventArgs e)
        {

        }

        private void label1_Click_3(object sender, EventArgs e)
        {

        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }


        private void angle0textbox_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.KeyCode == Keys.Enter)
            {
              
            }
        }

        private void acceleration0textbox_KeyDown(object sender, KeyEventArgs e)
        {
            if(e.KeyCode == Keys.Enter)
            {
         
            }
        }

        private void label8_Click(object sender, EventArgs e)
        {

        }

        private void label8_Click_1(object sender, EventArgs e)
        {

        }

        private void openPort_Click(object sender, EventArgs e)
        {
              if (serial_port.IsOpen == false)
            {
                serial_port.BaudRate = Convert.ToInt32(cboBaudRate.Text);
                serial_port.PortName = Convert.ToString(cboComPort.Text);
                serial_port.StopBits = StopBits.One;
                serial_port.DataBits = 8;
                serial_port.ReadTimeout = 500;
                serial_port.WriteTimeout = 500;
                try
                {


                    serial_port.Open();
                    txbStatus.Text = "Connected";
                }

                catch (IOException ioe)
                {
                    serial_port.Dispose();
                    txbStatus.Text = "Not Connected ";
                }
                serial_port.DataReceived += new SerialDataReceivedEventHandler(rx_handler);
                
            }
        }

        private void label9_Click(object sender, EventArgs e)
        {

        }

        private void angle0textbox_TextChanged(object sender, EventArgs e)
        {
              int i = 0;
                if(int.TryParse(angle0textbox.Text,out i) == true)
                {
                    int old_angle = stepperMotor0.current_angle;
                    if( stepperMotor0.SetAngle(i) == false)
                    {
                        angle0textbox.BackColor = Color.Red;
                    }
                    else
                    {
                        angle0textbox.BackColor = Color.White;
                        stepperMotor0.CalculateDuration(0, old_angle);
                        duration0textbox.Text = stepperMotor0.duration.ToString();
                    }
                }
                else
                {
                    angle0textbox.BackColor = Color.Red;
                }
        }

        private void acceleration0textbox_TextChanged(object sender, EventArgs e)
        {
                       int i = 0;
                if(int.TryParse(acceleration0textbox.Text,out i) == true)
                {
                    if( stepperMotor0.SetAcceleration(i) == false)
                    {
                        acceleration0textbox.BackColor = Color.Red;
                    }
                    else
                    {
                        acceleration0textbox.BackColor = Color.White;
                        // Console.WriteLine(stepperMotor0.acceleration);
                        
                        //serial_port.Write(str);
                    }
                }
                else
                {
                    acceleration0textbox.BackColor = Color.Red;
                }
        }

        private void btnUpdate0_Click(object sender, EventArgs e)
        {
            if(stepperMotor0.isAccel == true)
            {
                string str = "Ta" + stepperMotor0.acceleration.ToString() + "\n";
                serial_port.Write(str);
                stepperMotor0.isAccel = false;
            }
            if(stepperMotor0.isDuration == true)
            {
                string str = "Td" + stepperMotor0.duration.ToString() + "\n";
                serial_port.Write(str);
                stepperMotor0.isDuration = false;
            }
            if(stepperMotor0.isAngle == true)
            {
                string str = "Ta" + stepperMotor0.acceleration.ToString() + "\n";
                serial_port.Write(str);
                stepperMotor0.isAngle = false;
            }
            
        }
    }
}
