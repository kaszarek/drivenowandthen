import bb.cascades 1.3
import MyTimer 1.0

Container {
    id: root
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Top
    overlapTouchPolicy: OverlapTouchPolicy.Allow
    topPadding: ui.du(2.2)
    leftPadding: ui.du(2.2)
    rightPadding: ui.du(2.2)
    bottomPadding: ui.du(2.2)
    background: Color.create("#aa007ea4")
    
    signal reservationRequested()
    
    function setStatus(statusToSet){
        status.text = statusToSet;
    }
    
    function resetTimeLeft(){
        reservationControls.startTime = 0;        
    }
    
    function makeReservationVisible(state){
        reservationControls.visible = state
    }
    
    function makeLoginControllsVisible(state){
        loginControls.visible = state
        loginControls.enabled = true;
    }
    
    function setRegisterLoginAvailable(state){
        initialButtons.visible = state
    }
    
    function playBounceAnimation(){
        animFocus.play();
        animUnfocus.play();
    }
    
    function reservationStarted(){
        gettingResponse.visible = false;
        gettingResponse.stop();
        reserveBtn.visible = false;
        reserveBtn.enabled = true;
        cancelBtn.visible = true;
        timeLeft.visible = true;
        reservationElapse.start();
    }
    
    function reservationCancelledSucesfully(){
        cancelBtn.enabled = true;
        cancelBtn.visible = false;
        reserveBtn.visible = true;
        reserveBtn.enabled = true;
        timeLeft.visible = false;
        timeLeft.reset();
        reservationElapse.stop();
        resetTimeLeft()
    }
    
    function reservationDeclined(){
        gettingResponse.visible = false;
        gettingResponse.stop();
        resetTimeLeft();
        cancelBtn.visible = false;
        timeLeft.visible = false;
        reserveBtn.visible = true;
        reserveBtn.enabled = true;
    }
    
    function  reservationCancellationDeclined(){
        cancelBtn.enabled = true;
        cancellationProblem.show();
    }
    
    function cleanUp(){
        gettingResponse.visible = false;
        gettingResponse.stop();
        resetTimeLeft()
    }
    
    function startGettingCars(){
        carsLoading.visible = true
        rotate.play()
    }
    
    function stopGettingCars(){
        rotate.stop();
        carsLoading.visible = false
    }
    
    Container {
        // for debug purposes
        onTouch: {
            if (event.isUp()) {
                status.touched ++;
                if (status.touched >= 10) {
                    status.touched = 0;
                    status.visible = !status.visible;
                }
            }
        }
        Container {
            layout: StackLayout {
                orientation:  LayoutOrientation.LeftToRight
            }
            Container {
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 9.0
                }
                Container {
                    visible: false
                    id: initialButtons
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Button {
                        text: "Register"
                        color: Color.create("#a2c22d")
                        onClicked: {
                            Qt.openUrlExternally("https://de.drive-now.com/en/#!/register/1");
                        }
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 1.0
                        }
                    }
                    Button {
                        id: initialLoginButton
                        color: Color.Transparent
                        text: "Login"
                        onClicked: {
                            initialButtons.visible = false;
                            loginControls.visible = true;
                        }
                        animations: [
                            ScaleTransition {
                                id: animFocus
                                fromX: 1
                                toX: 1.2
                                fromY: 1
                                toY: 1.25
                                duration: 300
                                easingCurve: StockCurve.BounceOut
                            },
                            ScaleTransition {
                                id: animUnfocus
                                fromX: 1.2
                                toX: 1
                                fromY: 1.25
                                toY: 1
                                duration: 300
                                easingCurve: StockCurve.BounceOut
                            }
                        ]
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 1.0
                        }
                    }
                }
                Container {
                    id: loginControls
                    visible: false
                    verticalAlignment: VerticalAlignment.Center
                    Container {
                        TextField {
                            id: userName
                            horizontalAlignment: HorizontalAlignment.Fill
                            leftPadding: ui.du(2)
                            rightPadding: ui.du(2)
                            hintText: "E-mail address"
                        }
                        TextField {
                            id: password
                            horizontalAlignment: HorizontalAlignment.Fill
                            leftPadding: ui.du(2)
                            rightPadding: ui.du(2)
                            hintText: "Password"
                            inputMode: TextFieldInputMode.Password
                        }
                    }
                    onEnabledChanged: {
                        checkBox.enabled = enabled;
                        label.enabled = enabled;
                    }
                    Container {
                        topPadding: 15.0
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            Container {
                                id: customCheckBox
                                verticalAlignment: VerticalAlignment.Center
                                horizontalAlignment: HorizontalAlignment.Center
                                layout: StackLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 1
                                }
                                CheckBox {
                                    id: checkBox
                                    checked: false
                                }
                                Label {
                                    id: label
                                    leftMargin: 10
                                    text: "Keep me signed"
                                    onTouch: {
                                        if (event.isUp() && enabled) {
                                            checkBox.checked = ! checkBox.checked;
                                        }
                                    }
                                    verticalAlignment: VerticalAlignment.Center
                                }
                            }
                            Button {
                                id: loginButton
                                objectName: "loginBtn"
                                text: "Login"
                                horizontalAlignment: HorizontalAlignment.Right
                                verticalAlignment: VerticalAlignment.Center
                                signal onClickedWithParams(string user, string pass)
                                onClicked: {
                                    if (checkBox.checked) {
                                        userData.save("pass", password.text);
                                        userData.save("user", userName.text);
                                    }
                                    _mapViewTest.login(userName.text, password.text);
                                    loginControls.enabled = false;
                                }
                                layoutProperties: StackLayoutProperties {
                                    spaceQuota: 2
                                }
                            }
                        }
                    }
                }
                Container {
                    id: reservationControls
                    visible: false
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Button {
                        id: reserveBtn
                        horizontalAlignment: HorizontalAlignment.Center
                        text: "Reserve"
                        color: Color.create("#acca3e")
                        onClicked: {
                            reservationRequested();
                            gettingResponse.start();
                            _reservationController.bookCar(pinContainer.reservedPin.carId);
                            reserveBtn.enabled = false;
                        }
                    }
                    ActivityIndicator {
                        id: gettingResponse
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        visible: false
                        accessibility.name: "gettingResponse"
                    }
                    Button {
                        id: cancelBtn
                        visible: false
                        horizontalAlignment: HorizontalAlignment.Fill
                        text: "Cancel"
                        color: Color.create("#ec1414")
                        onClicked: {
                            _reservationController.cancelReservation();
                            enabled = false;
                        }
                    }
                    Label {
                        id: timeLeft
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        text: "15:00"
                        visible: false
                        textStyle {
                            base: tsd.style
                        }
                        function reset() {
                            text = "15:00";
                        }
                    }
                    property int maxTime: 900
                    property int elapsedSeconds
                    property int leftMinutes
                    property int leftSeconds
                    property double startTime
                    function onTimerTick() {
                        if (startTime == 0) {
                            startTime = new Date().getTime();
                        }
                        var currenTime = new Date().getTime();
                        elapsedSeconds = (currenTime - startTime) / 1000;
                        leftSeconds = parseInt(((maxTime - elapsedSeconds) % 60));
                        leftMinutes = Math.floor((maxTime - elapsedSeconds) / 60);
                        timeLeft.text = qsTr("%1:%2").arg(leftMinutes).arg(("0" + leftSeconds).slice(-2));
                        activeFrame.updateTimeLeft(timeLeft.text);
                        // TODO: there seems to be a bug here
                        if (elapsedSeconds >= maxTime) {
                            reservationElapse.stop();
                            startTime = 0;
                            timeLeft.visible = false;
                            cancelBtn.visible = false;
                            reserveBtn.visible = true;
                            reserveBtn.enabled = true;
                            pinContainer.reservationIsOngoing = false;
                            reservationExpired.show();
                            timeLeft.reset();
                        }
                    }
                    attachedObjects: [
                        QTimer {
                            id: reservationElapse
                            interval: 1000
                            onTimeout: {
                                reservationControls.onTimerTick();
                            }
                        },
                        QTimer {
                            id: statusClear
                            interval: 4000
                            singleShot: true
                            onTimeout: {
                                status.cleared = true;
                                status.text = "";
                            }
                        },
                        TextStyleDefinition {
                            id: tsd
                            base: SystemDefaults.TextStyles.BigText
                            fontSize: FontSize.XLarge
                            fontWeight: FontWeight.Bold
                        }
                    ]
                }
            }
            Container {
                id: carsLoading
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1.0
                }
                visible: false
                horizontalAlignment: HorizontalAlignment.Right
                ImageView {
                    imageSource: "asset:///images/loading_car.png"
                    horizontalAlignment: HorizontalAlignment.Right
                    animations: [
                        RotateTransition {
                            id: rotate
                            fromAngleZ: 0
                            toAngleZ: 360
                            duration: 2000
                            repeatCount: 1000
                        }
                    ]
                    onCreationCompleted: {
                        rotate.play();
                    }
                }
            }
        
        }
        Label {
            id: status
            property int touched: 0
            multiline: true
            visible: false
            property bool cleared: false
            textStyle {
                base: SystemDefaults.TextStyles.SmallText
                color: Color.Black
                fontWeight: FontWeight.Bold
            }
            onTextChanged: {
                if (! cleared) {
                    statusClear.start();
                } else {
                    cleared = false;
                }
            }
        }
    }
}
