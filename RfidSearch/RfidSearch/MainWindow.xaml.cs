using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace RfidSearch
{
    /// <summary>
    /// Логика взаимодействия для MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        SerialPort serialPort;
        private Queue<char> RX_data;    //буфер для принятых данных
        Task send;                      //поток для приняти данных

        public MainWindow()
        {
            InitializeComponent();
            serialPort = new SerialPort();

            RX_data = new Queue<char>();
            send = new Task(() => { });
            send.Start();
            try
            {
                serialPort.BaudRate = 9600;
                serialPort.PortName = "COM8";
                serialPort.DataReceived += serialPort_DataReceived;
                serialPort.Open();
            }
            catch (Exception)
            {
                lConsole.Content = "ошибка";
            }

            string mes = "test";
            // TranslateTransform translateTransform1 = new TranslateTransform(50 + count * 2, 20);
            send = Task.Run(new Action(() =>
            {

                //Dispatcher.BeginInvoke(new Action<TranslateTransform>(delegate (TranslateTransform t1)
                //{
                //    lConsole.Text = mes;
                //    // this.UpdateLayout();
                //}), System.Windows.Threading.DispatcherPriority.Normal);
                this.lConsole.Dispatcher.Invoke(new Action(
                  () =>
                  {
                      //TranslateTransform translateTransform1 = new TranslateTransform(50 + count * 2, 20);
                      lConsole.Content = mes;
                  }));
            }));
            //Dispatcher.BeginInvoke(new Action(
            //{
            //    lConsole.Text = mes;
            //    // this.UpdateLayout();
            //}), System.Windows.Threading.DispatcherPriority.Normal));
        }

        private void serialPort_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            try
            {
                SerialPort sp = (SerialPort)sender;
                while (0 != sp.BytesToRead) RX_data.Enqueue((char)sp.ReadByte());
                if (send.IsCompleted)
                    send = Task.Run(new Action(() =>
                    {
                        try
                        {
                            string mes = "";
                            Thread.Sleep(50);     //ожидаем завершения передачи
                            foreach (char c in RX_data)
                            {
                                //this.Invoke(new Action(() => { Console.Text += c; }));
                                mes += c;
                            }

                            string UID = findText(mes, "UID");
                            string Name = findText(mes, "name");
                            //if (Name.Length > 1)
                            //{
                                this.lConsole.Dispatcher.Invoke(new Action(
                                () =>
                                {
                                    lConsole.Content = UID;
                                }));

                                this.lName.Dispatcher.Invoke(new Action(
                                () =>
                                {
                                    lName.Content = Name;
                                }));
                                this.lName_Copy1.Dispatcher.Invoke(new Action(
                               () =>
                               {
                                   lName_Copy1.Content = mes;
                               }));
                            
                            //}
                            RX_data = new Queue<char>();
                        }
                        catch (Exception te)
                        {
                            //this.Invoke(new Action(() => { Console.Text += te.Message + '\n'; })); 
                            // TranslateTransform translateTransform1 = new TranslateTransform();
                            this.lConsole.Dispatcher.Invoke(new Action(
                            () =>
                            {
                                lConsole.Content = te.Message;
                            }));

                        }

                    }));


            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString(), "Ошибка");
            }
        }



        private void Button_Click(object sender, RoutedEventArgs e)
        {

        }

        public static String findText(String str, String findContext)
        {
            int find1 = str.IndexOf("<" + findContext + ">");
            int find2 = str.IndexOf("</" + findContext + ">");
            String findStr = "";
            for (int i = find1 + ("<" + findContext + ">").Length; i < find2; i++)
            {
                findStr += (char)str[i];
            }

            return findStr;
        }
    }
}
