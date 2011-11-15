#include "udpclient.h"

UdpClient::UdpClient(char* host)
{
	logger = Logger::instance();
	logger->UdpClient("UdpClient::UdpClient()");
	logger->UdpClient("UdpClient::UdpClient(): port is %d, or htons(%d).", 1234, htons(1234));

// 	socket = new QUdpSocket(this);
// 	socket->connectToHost("localhost", 1234);
	
	// create the UDP socket
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) abort("UdpClient::UdpClient(): couldn't open socket, aborting.");
	
	// initialize the server struct with zeroes
	bzero(&server,sizeof(server));
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(1234);
}

UdpClient::~UdpClient()
{
	close(sock);
}

void UdpClient::send(Packet *packet)
{
	//ignore the recipient in the packet, its being set in UdpClient's c'tor.

	//logger->UdpClient("UdpClient::send(): about to send a packet with a length of: %d.", packet->getPacketLength());
	
	packet->finalize();
	
	int size = sendto(
			sock,
			packet->getData(),
			packet->getPacketLength(),
			0,
			(struct sockaddr *)&server,
			sizeof(server));

	//logger->UdpClient("UdpClient::send(): sendto() returned %d.", size);

	if(size < 0) abort("UdpClient::send(): sendto() failed, aborting, sendto() answered %d.", size);
}

bool UdpClient::receive(Packet* packet)
{
	// we don't care where the packet was from, so pass NULL.
	//int n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *)&from, (socklen_t *)&fromlen);
	int numberOfBytesReceived = recvfrom(sock, buf, 1024, 0, NULL, NULL);
	//logger->UdpClient("UdpClient::receive(void): received %d bytes from server, building a packet from this data.", numberOfBytesReceived);
		
	if(numberOfBytesReceived < 0) abort("UdpServer::run(): recvfrom() failed, aborting.");

	packet->setData(buf, numberOfBytesReceived);
	
	return true;
}

void UdpClient::run(void)
{
	while(true)
	{
		// TOGGLE EMERGENCY STOP ON/OFF
		logger->UdpClient("UdpClient::run(): enabling EMSTOP.");
		Packet brakeOn(CAN_REQUEST | CAN_EMERGENCY_STOP_ENABLE);
		send(&brakeOn);

		usleep(2000000);

		logger->UdpClient("UdpClient::run(): disabling EMSTOP.");
		Packet brakeOff(CAN_REQUEST | CAN_EMERGENCY_STOP_DISABLE);
		send(&brakeOff);

		usleep(2000000);
	}

	while(true)
	{
		// TOGGLE BRAKES ON/OFF
		logger->UdpClient("UdpClient::run(): enabling brakes.");
		Packet brakeOn(CAN_REQUEST | CAN_BRAKES_ENABLE);
		send(&brakeOn);

		usleep(200000);

		logger->UdpClient("UdpClient::run(): disabling brakes.");
		Packet brakeOff(CAN_REQUEST | CAN_BRAKES_DISABLE);
		send(&brakeOff);

		usleep(200000);
	}
	
	while(false)
	{
	        Packet request(CAN_REQUEST | CAN_REMOTECONTROL);
	        send(&request);

		Packet* response = new Packet();
		receive(response);

		if(!response->isValid())
			abort("received an invalid packet!");

		int steering = response->popS32();
		int speed = response->popS32();
		delete response;

		logger->UdpClient("input: speed %d, steering %d.", speed, steering);

		// speed should be between -46656 and 46656 (-36 to 36 ^ 3)
		// maximum Speed in m/s.
		float maximumTranslationSpeed = 1.0;

		float speedLeft, speedRight;

		speedLeft = maximumTranslationSpeed / (double)46656 * (double)speed;
		speedRight= maximumTranslationSpeed / (double)46656 * (double)speed;

		logger->UdpClient("before steering: speedLeft %2.2f, speedRight %2.2f", speedLeft, speedRight);

		float maximumRotationSpeed = maximumTranslationSpeed / 2.5;

		// steering should be between -36 and 36 (-36 to 36 ^ 3)
		float steeringFactor = (maximumRotationSpeed / (double)36.0 * (double)steering);
		logger->UdpClient("steeringFactor is %2.2f", steeringFactor);
		// steeringfactor is -1 to +1

		speedLeft = speedLeft + steeringFactor;
		speedRight = speedRight - steeringFactor;

		logger->UdpClient("setting speed to %2.2f left, %2.2f right", speedLeft, speedRight);

		Packet requestSpeed(CAN_REQUEST | CAN_SET_WHEELSPEEDS);
		requestSpeed.pushF32(speedLeft);
		requestSpeed.pushF32(speedRight);
		send(&requestSpeed);

		usleep(50000);
	}
	
	
	while(true)
	{
		// READ TEMPERATURES
		Packet request(CAN_REQUEST | CAN_MOTORTEMPS);
		send(&request);

		Packet* response = new Packet();
		receive(response);

		float temperatures[2];
		temperatures[0] = response->popF32();
		temperatures[1] = response->popF32();

		if(response->isValid())
			logger->UdpClient("received a packet with temperatures %2.2f�C / %2.2f�C", temperatures[0], temperatures[1]);

		delete response;

		usleep(50000);
	}
	
	while(false)
	{
		// READ BATTERY
		Packet request(CAN_REQUEST | CAN_BATTERYVOLTAGE);
		send(&request);

		Packet* response = new Packet();
		receive(response);

		float voltage = response->popF32();

		if(response->isValid())
			logger->UdpClient("received a packet with voltage %2.2fV", voltage);

		delete response;

		usleep(50000);
	}

	while(false)
	{
		// READ WHEEL ADVANCES
		Packet request(CAN_REQUEST | CAN_WHEELADVANCES);
		send(&request);

		Packet* response = new Packet();
		receive(response);

		int angleDeltaLeft, angleDeltaRight;
		angleDeltaLeft = response->popS32();
		angleDeltaRight = response->popS32();

		if(response->isValid())
			logger->UdpClient("received a packet with wheelAdvances %d / %d", angleDeltaLeft, angleDeltaRight);

		delete response;

		usleep(50000);
	}
	
	while(false)
	{
		// SET MOTOR SPEEDS
		Packet requestSpeed(CAN_REQUEST | CAN_SET_WHEELSPEEDS);
		requestSpeed.pushS32(100000);
		requestSpeed.pushS32(100000);
		send(&requestSpeed);

		usleep(50000);
	}
	
}

int main(int argc, char**argv)
{
	Logger::setupLogging(argc, argv);

	UdpClient client(argv[1]);

	client.run();
}
