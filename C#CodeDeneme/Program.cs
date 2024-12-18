using System;
using System.Text;
using System.Net.Sockets;

namespace client
{
    internal class Program
    {
        static void Main(string[] args)
        {
            string serverAddress = "127.0.0.1"; // Server address (localhost)
            int port = 8080; // Target port

            try
            {
                while (true)
                {
                    // Prompt user for a command
                    Console.Write("Enter a command (or type 'exit' to quit): ");
                    string command = Console.ReadLine();

                    // Exit if the user types 'exit'
                    if (command?.ToLower() == "exit")
                    {
                        Console.WriteLine("Exiting...");
                        break;
                    }

                    // Create a TCP client and connect to the server
                    using (TcpClient client = new TcpClient(serverAddress, port))
                    {
                        Console.WriteLine($"Connected to {serverAddress}:{port}");

                        // Get the network stream
                        NetworkStream stream = client.GetStream();

                        // Convert the command to bytes and send it
                        byte[] dataToSend = Encoding.UTF8.GetBytes(command);
                        stream.Write(dataToSend, 0, dataToSend.Length);
                        Console.WriteLine($"Sent: {command}");

                        // Read the response from the server
                        byte[] receivedData = new byte[1024];
                        int bytesRead = stream.Read(receivedData, 0, receivedData.Length);
                        string response = Encoding.UTF8.GetString(receivedData, 0, bytesRead);
                        Console.WriteLine($"Server response: {response}");
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"An error occurred: {ex.Message}");
            }
        }
    }
}
