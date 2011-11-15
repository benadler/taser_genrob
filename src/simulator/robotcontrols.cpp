#include "robotcontrols.h"

RobotControls::RobotControls() : QDockWidget(), Ui_RobotControl()
{
	setupUi(this);
	setWidget(topWidget);

	maxSpeed = 2000000;

	speedBoxLeft->setMaximum(maxSpeed);
	speedBoxRight->setMaximum(maxSpeed);
	speedBoxLeft->setMinimum(-maxSpeed);
	speedBoxRight->setMinimum(-maxSpeed);

	connect(speedBoxLeft, SIGNAL(valueChanged(double)), this, SLOT(slotSpeedBoxValuesChanged()));
	connect(speedBoxRight, SIGNAL(valueChanged(double)), this, SLOT(slotSpeedBoxValuesChanged()));

	connect(poseX, SIGNAL(valueChanged(double)), this, SLOT(slotPoseValuesChanged()));
	connect(poseY, SIGNAL(valueChanged(double)), this, SLOT(slotPoseValuesChanged()));
	connect(poseAngle, SIGNAL(valueChanged(double)), this, SLOT(slotPoseValuesChanged()));

	connect(btnUpdateCurves, SIGNAL(clicked()), this, SLOT(slotUpdateCurves()));

	// set initial values, so that we read valid stuff.
	batteryVoltage->setValue(48.0);

	motorTempLeft->setValue(25.0);
	motorTempRight->setValue(25.0);

	setMotorSpeeds(0, 0);

	checkBoxBrakes->setCheckState(Qt::Checked);
}

RobotControls::~RobotControls()
{
}

void RobotControls::slotPoseValuesChanged(void)
{
	emit newPose(poseX->value(), poseY->value(), poseAngle->value());
}

void RobotControls::slotSetPose(double x, double y, double angle)
{
// 	qDebug("RobotControls::slotSetPose(): %.2F, %.2F, %.2F\n", x, y, angle);
	poseX->blockSignals(true);
	poseY->blockSignals(true);
	poseAngle->blockSignals(true);

	poseX->setValue(x);
	poseY->setValue(y);
	poseAngle->setValue(angle);

	poseX->blockSignals(false);
	poseY->blockSignals(false);
	poseAngle->blockSignals(false);
}

void RobotControls::slotSpeedBoxValuesChanged(void)
{
	speedBarLeft->setValue(abs((int)(speedBoxLeft->value() / maxSpeed * 1000000 * 100)));
	speedBarRight->setValue(abs((int)(speedBoxRight->value() / maxSpeed * 1000000 * 100)));
	speedLeft = (int)(speedBoxLeft->value() * 1000000);
	speedRight = (int)(speedBoxRight->value() * 1000000);
}

void RobotControls::setMotorSpeeds(int left, int right)
{
	// all values in micrometer/second
	speedLeft = left;
	speedRight = right;
	speedBoxLeft->setValue(left / 1000000.0);
	speedBoxRight->setValue(right / 1000000.0);

	slotSpeedBoxValuesChanged();
}

void RobotControls::setBrakes(const bool enable)
{
	if(enable)
		checkBoxBrakes->setCheckState(Qt::Checked);
	else
		checkBoxBrakes->setCheckState(Qt::Unchecked);
}

void RobotControls::getMotorSpeeds(int &left, int &right) const
{
	// all values in micrometer/second
	left = speedLeft;
	right = speedRight;
}

void RobotControls::getMotorTemperatures(float &left, float &right) const
{
	// return values in °C
	left = motorTempLeft->value();
	right = motorTempRight->value();
}

float RobotControls::getBatteryVoltage(void) const
{
	return batteryVoltage->value();
}

void RobotControls::getRemoteControl(int &steering, int &speed) const
{
	steering = remoteControlSteering->value();
	speed = (int)pow(remoteControlSpeed->value(), 3);
}

void RobotControls::getMotorAdvances(int callerID, int &left, int &right)
{
	int millisecondsBetweenCalls = 0;

	// Check whether there is a last-call-timestamp for this caller. If yes,
	// then use it. If no, then create one and use it.
	if(timeStamps.value(callerID).isValid())
	{
		// Find out how much time elapsed between the last call and this call
		millisecondsBetweenCalls = timeStamps[callerID].msecsTo(QTime::currentTime());
		timeStamps[callerID] = QTime::currentTime();
	}
	else
	{
		// First call from the caller
		timeStamps[callerID] = QTime::currentTime();
		millisecondsBetweenCalls = timeStamps[callerID].msecsTo(QTime::currentTime());
	}

	// we have the speed, we have the time. Get the distances
	getMotorSpeeds(left, right);
	//qDebug() << "speed left / right: " << left << right;

	left = left * millisecondsBetweenCalls / 1000;
	right = right * millisecondsBetweenCalls / 1000;
	//qDebug() << "timediff:" << millisecondsBetweenCalls << "advance left / right: " << left << right;
}

void RobotControls::slotUpdateCurves(void)
{
	// Create a QList of QPointFs from the text field
	QList<QPointF> points;
	QString text = textCurve->toPlainText();
	QStringList lines = text.split("\n", QString::SkipEmptyParts, Qt::CaseInsensitive);

	int lineCount = 0;
	foreach (QString line, lines)
	{
		lineCount++;
		if(line.isEmpty()) continue;

		bool xOK = true, yOK = true;

		QStringList coords = line.split(" / ", QString::SkipEmptyParts, Qt::CaseInsensitive);
		QPointF point(coords.at(0).toFloat(&xOK), coords.at(1).toFloat(&yOK));

		if(!xOK || !yOK)
		{
			QErrorMessage().showMessage(QString("Parse Error at line ").append(QString::number(lineCount)));
			return;
		}

		// weak attempt to normalize to meters.
		if(point.x() > 100000.0) point /= 1000000.0;
		if(point.x() > 100.0) point /= 1000.0;

		points << point;
	}

	// Now we have a list of points. emit the spline-build-signals for robotscene to chew on.
	emit showSplineCubic(chkShowCubic->isChecked(), points);
	emit showSplineHermite(chkShowHermite->isChecked(), points, hermiteTension->value(), hermiteBias->value());
}
