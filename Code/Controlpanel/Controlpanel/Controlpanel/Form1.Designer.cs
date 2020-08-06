namespace Controlpanel
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.panel1 = new System.Windows.Forms.Panel();
            this.acceleration0textbox = new System.Windows.Forms.TextBox();
            this.duration0textbox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.angle0textbox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.StepperMotor0 = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.acceleration1textbox = new System.Windows.Forms.TextBox();
            this.duration1textbox = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.angle1textbox = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.comlabel = new System.Windows.Forms.Label();
            this.cboComPort = new System.Windows.Forms.ComboBox();
            this.baudratelabel = new System.Windows.Forms.Label();
            this.cboBaudRate = new System.Windows.Forms.ComboBox();
            this.txbStatus = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.openPort = new System.Windows.Forms.Button();
            this.richTextBox1 = new System.Windows.Forms.RichTextBox();
            this.rtbDataReceived = new System.Windows.Forms.Label();
            this.btnUpdate0 = new System.Windows.Forms.Button();
            this.btnUpdate1 = new System.Windows.Forms.Button();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.btnUpdate0);
            this.panel1.Controls.Add(this.acceleration0textbox);
            this.panel1.Controls.Add(this.duration0textbox);
            this.panel1.Controls.Add(this.label2);
            this.panel1.Controls.Add(this.angle0textbox);
            this.panel1.Controls.Add(this.label3);
            this.panel1.Controls.Add(this.label1);
            this.panel1.Controls.Add(this.StepperMotor0);
            this.panel1.Location = new System.Drawing.Point(7, 309);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(148, 129);
            this.panel1.TabIndex = 0;
            // 
            // acceleration0textbox
            // 
            this.acceleration0textbox.Location = new System.Drawing.Point(107, 48);
            this.acceleration0textbox.Name = "acceleration0textbox";
            this.acceleration0textbox.Size = new System.Drawing.Size(38, 20);
            this.acceleration0textbox.TabIndex = 6;
            this.acceleration0textbox.TextChanged += new System.EventHandler(this.acceleration0textbox_TextChanged);
            this.acceleration0textbox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.acceleration0textbox_KeyDown);
            // 
            // duration0textbox
            // 
            this.duration0textbox.Location = new System.Drawing.Point(107, 77);
            this.duration0textbox.Name = "duration0textbox";
            this.duration0textbox.Size = new System.Drawing.Size(38, 20);
            this.duration0textbox.TabIndex = 5;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(15, 80);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(47, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Duration";
            // 
            // angle0textbox
            // 
            this.angle0textbox.Location = new System.Drawing.Point(107, 22);
            this.angle0textbox.Name = "angle0textbox";
            this.angle0textbox.Size = new System.Drawing.Size(38, 20);
            this.angle0textbox.TabIndex = 4;
            this.angle0textbox.TextChanged += new System.EventHandler(this.angle0textbox_TextChanged);
            this.angle0textbox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.angle0textbox_KeyDown);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(15, 55);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(66, 13);
            this.label3.TabIndex = 3;
            this.label3.Text = "Acceleration";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(15, 29);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(34, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Angle";
            this.label1.Click += new System.EventHandler(this.label1_Click_3);
            // 
            // StepperMotor0
            // 
            this.StepperMotor0.AutoSize = true;
            this.StepperMotor0.Location = new System.Drawing.Point(15, 0);
            this.StepperMotor0.Name = "StepperMotor0";
            this.StepperMotor0.Size = new System.Drawing.Size(83, 13);
            this.StepperMotor0.TabIndex = 0;
            this.StepperMotor0.Text = "Stepper Motor 0";
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.btnUpdate1);
            this.panel2.Controls.Add(this.acceleration1textbox);
            this.panel2.Controls.Add(this.duration1textbox);
            this.panel2.Controls.Add(this.label4);
            this.panel2.Controls.Add(this.angle1textbox);
            this.panel2.Controls.Add(this.label5);
            this.panel2.Controls.Add(this.label6);
            this.panel2.Controls.Add(this.label7);
            this.panel2.Location = new System.Drawing.Point(161, 309);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(148, 129);
            this.panel2.TabIndex = 1;
            // 
            // acceleration1textbox
            // 
            this.acceleration1textbox.Location = new System.Drawing.Point(107, 51);
            this.acceleration1textbox.Name = "acceleration1textbox";
            this.acceleration1textbox.Size = new System.Drawing.Size(38, 20);
            this.acceleration1textbox.TabIndex = 6;
            // 
            // duration1textbox
            // 
            this.duration1textbox.Location = new System.Drawing.Point(107, 77);
            this.duration1textbox.Name = "duration1textbox";
            this.duration1textbox.Size = new System.Drawing.Size(38, 20);
            this.duration1textbox.TabIndex = 5;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(15, 51);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(66, 13);
            this.label4.TabIndex = 3;
            this.label4.Text = "Acceleration";
            // 
            // angle1textbox
            // 
            this.angle1textbox.Location = new System.Drawing.Point(107, 22);
            this.angle1textbox.Name = "angle1textbox";
            this.angle1textbox.Size = new System.Drawing.Size(38, 20);
            this.angle1textbox.TabIndex = 4;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(15, 80);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(47, 13);
            this.label5.TabIndex = 2;
            this.label5.Text = "Duration";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(15, 29);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(34, 13);
            this.label6.TabIndex = 1;
            this.label6.Text = "Angle";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(15, 0);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(83, 13);
            this.label7.TabIndex = 0;
            this.label7.Text = "Stepper Motor 1";
            // 
            // comlabel
            // 
            this.comlabel.AutoSize = true;
            this.comlabel.Location = new System.Drawing.Point(7, 13);
            this.comlabel.Name = "comlabel";
            this.comlabel.Size = new System.Drawing.Size(31, 13);
            this.comlabel.TabIndex = 2;
            this.comlabel.Text = "COM";
            // 
            // cboComPort
            // 
            this.cboComPort.FormattingEnabled = true;
            this.cboComPort.Location = new System.Drawing.Point(74, 10);
            this.cboComPort.Name = "cboComPort";
            this.cboComPort.Size = new System.Drawing.Size(78, 21);
            this.cboComPort.TabIndex = 3;
            // 
            // baudratelabel
            // 
            this.baudratelabel.AutoSize = true;
            this.baudratelabel.Location = new System.Drawing.Point(7, 37);
            this.baudratelabel.Name = "baudratelabel";
            this.baudratelabel.Size = new System.Drawing.Size(53, 13);
            this.baudratelabel.TabIndex = 4;
            this.baudratelabel.Text = "Baud rate";
            // 
            // cboBaudRate
            // 
            this.cboBaudRate.FormattingEnabled = true;
            this.cboBaudRate.Location = new System.Drawing.Point(74, 34);
            this.cboBaudRate.Name = "cboBaudRate";
            this.cboBaudRate.Size = new System.Drawing.Size(78, 21);
            this.cboBaudRate.TabIndex = 5;
            // 
            // txbStatus
            // 
            this.txbStatus.Location = new System.Drawing.Point(74, 62);
            this.txbStatus.Name = "txbStatus";
            this.txbStatus.Size = new System.Drawing.Size(78, 20);
            this.txbStatus.TabIndex = 6;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(7, 65);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(37, 13);
            this.label8.TabIndex = 7;
            this.label8.Text = "Status";
            this.label8.Click += new System.EventHandler(this.label8_Click_1);
            // 
            // openPort
            // 
            this.openPort.Location = new System.Drawing.Point(74, 89);
            this.openPort.Name = "openPort";
            this.openPort.Size = new System.Drawing.Size(75, 23);
            this.openPort.TabIndex = 8;
            this.openPort.Text = "Open";
            this.openPort.UseVisualStyleBackColor = true;
            this.openPort.Click += new System.EventHandler(this.openPort_Click);
            // 
            // richTextBox1
            // 
            this.richTextBox1.Location = new System.Drawing.Point(576, 37);
            this.richTextBox1.Name = "richTextBox1";
            this.richTextBox1.Size = new System.Drawing.Size(186, 246);
            this.richTextBox1.TabIndex = 9;
            this.richTextBox1.Text = "";
            // 
            // rtbDataReceived
            // 
            this.rtbDataReceived.AutoSize = true;
            this.rtbDataReceived.Location = new System.Drawing.Point(576, 18);
            this.rtbDataReceived.Name = "rtbDataReceived";
            this.rtbDataReceived.Size = new System.Drawing.Size(77, 13);
            this.rtbDataReceived.TabIndex = 10;
            this.rtbDataReceived.Text = "Data received ";
            this.rtbDataReceived.Click += new System.EventHandler(this.label9_Click);
            // 
            // btnUpdate0
            // 
            this.btnUpdate0.Location = new System.Drawing.Point(18, 103);
            this.btnUpdate0.Name = "btnUpdate0";
            this.btnUpdate0.Size = new System.Drawing.Size(75, 23);
            this.btnUpdate0.TabIndex = 7;
            this.btnUpdate0.Text = "Update";
            this.btnUpdate0.UseVisualStyleBackColor = true;
            this.btnUpdate0.Click += new System.EventHandler(this.btnUpdate0_Click);
            // 
            // btnUpdate1
            // 
            this.btnUpdate1.Location = new System.Drawing.Point(18, 102);
            this.btnUpdate1.Name = "btnUpdate1";
            this.btnUpdate1.Size = new System.Drawing.Size(75, 23);
            this.btnUpdate1.TabIndex = 7;
            this.btnUpdate1.Text = "Update";
            this.btnUpdate1.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.rtbDataReceived);
            this.Controls.Add(this.richTextBox1);
            this.Controls.Add(this.openPort);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.txbStatus);
            this.Controls.Add(this.cboBaudRate);
            this.Controls.Add(this.baudratelabel);
            this.Controls.Add(this.cboComPort);
            this.Controls.Add(this.comlabel);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label StepperMotor0;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox acceleration0textbox;
        private System.Windows.Forms.TextBox duration0textbox;
        private System.Windows.Forms.TextBox angle0textbox;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.TextBox acceleration1textbox;
        private System.Windows.Forms.TextBox duration1textbox;
        private System.Windows.Forms.TextBox angle1textbox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label comlabel;
        private System.Windows.Forms.ComboBox cboComPort;
        private System.Windows.Forms.Label baudratelabel;
        private System.Windows.Forms.ComboBox cboBaudRate;
        private System.Windows.Forms.TextBox txbStatus;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button openPort;
        private System.Windows.Forms.RichTextBox richTextBox1;
        private System.Windows.Forms.Label rtbDataReceived;
        private System.Windows.Forms.Button btnUpdate0;
        private System.Windows.Forms.Button btnUpdate1;
    }
}

