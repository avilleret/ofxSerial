// =============================================================================
//
// Copyright (c) 2014-2015 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#include "ofApp.h"


void ofApp::setup()
{
    // 1. Upload the HaikuGenerator.ino sketch (in this example's Arduino/
    //    folder) to an Arduino board.
    // 2. Check the "getDevices" call below to make sure the correct serial
    //    device is connected.  This works with OSX but may require a different
    //    port name for Linux or Windows.
    // 3. Run this app.

    ofEnableAlphaBlending();

    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();

    ofLogNotice("ofApp::setup") << "Connected Devices: ";

    for (std::size_t i = 0; i < devicesInfo.size(); ++i)
    {
        ofLogNotice("ofApp::setup") << "\t" << devicesInfo[i];
    }

    if (!devicesInfo.empty())
    {
        // Connect to the first matching device.
        bool success = device.setup(devicesInfo[0], 115200);

        if(success)
        {
            device.registerAllEvents(this);

            ofLogNotice("ofApp::setup") << "Successfully setup " << devicesInfo[0];
        }
        else
        {
            ofLogNotice("ofApp::setup") << "Unable to setup " << devicesInfo[0];
        }
    }
    else
    {
        ofLogNotice("ofApp::setup") << "No devices connected.";
    }
}

void ofApp::exit()
{
    device.unregisterAllEvents(this);
}


void ofApp::draw()
{
    ofBackground(0);

    ofSetColor(255);

    std::stringstream ss;

    ss << "         FPS: " << ofGetFrameRate() << std::endl;
    ss << "Connected to: " << device.getPortName();

    ofDrawBitmapString(ss.str(), ofVec2f(20, 20));

    std::vector<SerialMessage>::iterator iter = serialMessages.begin();

    int x = 20;
    int y = 50;
    int height = 20;

    while (iter != serialMessages.end())
    {
        iter->fade -= 1;

        if (iter->fade < 0)
        {
            iter = serialMessages.erase(iter);
        }
        else
        {
            ofSetColor(255, ofClamp(iter->fade, 0, 255));
            ofDrawBitmapString(iter->message, ofVec2f(x, y));

            y += height;

            if (!iter->exception.empty())
            {
                ofSetColor(255, 0, 0, ofClamp(iter->fade, 0, 255));
                ofDrawBitmapString(iter->exception, ofVec2f(x + height, y));
                y += height;
            }
            
            ++iter;
        }
    }



	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

	float angle;
	ofVec3f vec;
	quat.getRotate(angle, vec);
	ofRotate(angle, vec.x, vec.y, vec.z);

	ofDrawAxis(300);

//	ofDrawBox(0, 0, 0, 100, 200, 300);


	ofPopMatrix();

}

void ofApp::onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args)
{
    // Buffers will show up here when the marker character is found.
    SerialMessage message(args.getBuffer().toString(), "", 500);
   // serialMessages.push_back(message);

	std::vector<std::string> tokens = ofSplitString(args.getBuffer().toString(), ",", true);

//	cout << ofToString(tokens) << endl;

	if (tokens[0] == "q")
	{
		quat = ofQuaternion(ofToFloat(tokens[2]),ofToFloat(tokens[3]), ofToFloat(tokens[4]), ofToFloat(tokens[1]));
	}

}

void ofApp::onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args)
{
    // Errors and their corresponding buffer (if any) will show up here.
    SerialMessage message(args.getBuffer().toString(),
                          args.getException().displayText(),
                          500);
    serialMessages.push_back(message);
}
