using System;
using System.IO;
using System.IO.Ports;
using System.Windows.Forms;

namespace Comntador
{
    public partial class Inicio : Form
    {
        private SerialPort _serial;

        public Inicio()
        {
            InitializeComponent();
        }

        private void Inicio_Load(object sender, EventArgs e)
        {
            InitSerial();
            SendValue("COUNT");
        }

        private void SendValue(string value)
        {
            if (_serial != null && _serial.IsOpen)
            {
                try { _serial.WriteLine(value); } // mejor usar WriteLine para que incluya el NewLine
                catch (Exception ex)
                {
                    MessageBox.Show($"Error enviando '{value}': {ex.Message}",
                                    "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void Salir_Click(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        private static string GetConfiguredPort()
        {
            string baseDir = AppDomain.CurrentDomain.BaseDirectory;
            string configTxt = Path.Combine(baseDir, "config.txt");

            if (File.Exists(configTxt))
            {
                string line = File.ReadAllText(configTxt).Trim();
                if (!string.IsNullOrWhiteSpace(line))
                    return line;
            }

            string[] ports = SerialPort.GetPortNames();
            if (ports.Length > 0)
            {
                Array.Sort(ports);
                return ports[0];
            }

            return "COM3";
        }

        private void InitSerial()
        {
            string portName = GetConfiguredPort();

            if (_serial != null)
            {
                try
                {
                    if (_serial.IsOpen)
                        _serial.Close();
                }
                catch { /* Ignorar errores al cerrar */ }
            }

            _serial = new SerialPort(portName, 115200)
            {
                NewLine = "\n",
                ReadTimeout = 1000,
                WriteTimeout = 1000
            };

            _serial.DataReceived += Serial_DataReceived;

            try
            {
                _serial.Open();
                Text = $"Comntador  [Puerto: {portName}]";
            }
            catch (Exception ex)
            {
                MessageBox.Show($"No se pudo abrir el puerto {portName}: {ex.Message}",
                                "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void Serial_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                string data = _serial.ReadLine();
                LeerDesdeSerial(data);
            }
            catch (Exception) { /* leer fallido, ignorar */ }
        }

        // Función para manejar la entrada desde el puerto serial
        private void LeerDesdeSerial(string data)
        {
            BeginInvoke((Action)(() =>
            {
                if (data.StartsWith("COUNT:"))
                {
                    string[] partes = data.Split(':');
                    if (partes.Length == 2)
                    {
                        Contador.Text = partes[1];
                    }
                }
                if (data.StartsWith("RST"))
                {
                        Contador.Text = "0";
                }
                else
                {
                    Terminal.Text = data;
                }
            }));
        }

        private void Conectar_Click(object sender, EventArgs e)
        {
            InitSerial();
            SendValue("COUNT");
        }

        private void Reset_Click(object sender, EventArgs e)
        {
            DialogResult resultado = MessageBox.Show("Pondra el valor del contador en cero. ¿Querés continuar con el procedimiento?", "Confirmación", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);

            if (resultado == DialogResult.Yes)
            {
                SendValue("RST");
            }
        }
    }
}
