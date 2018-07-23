/*
 * Copyright (c) 2011-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.3
import bb.system 1.2
import QtMobility.sensors 1.2
import bb.cascades.maps 1.3
import QtMobilitySubset.location 1.1

NavigationPane {
    id: navPane 
    Page {
        id: mainPage
        property bool userLoggedIn: false 
        objectName: "mainPage"
        actionBarVisibility: ChromeVisibility.Compact;
        actions: [
            ActionItem {
                ActionBar.placement: ActionBarPlacement.InOverflow
                imageSource: "asset:///images/changeCity.png"
                title: "Change city"
                onTriggered: {
                    var cityPickerObject = cityPicker.createObject()
                    cityPickerObject.setNavigator(navPane)
                    navPane.push(cityPickerObject)
                }
            },
            ActionItem {
                ActionBar.placement: ActionBarPlacement.InOverflow
                imageSource: "asset:///images/refresh.png"
                title: "Refresh"
                onTriggered: {
                    pinContainer.removeAllPins();
                    _mapViewTest.getCars();
                    userControls.startGettingCars();
                }
            }
        ]
        Container {
            id: root
            layout: DockLayout {
            }
            onCreationCompleted: {
                meContainer.showMe();
                Application.thumbnail.connect(parent.onThumbnail());
//                userControls.setRegisterLoginAvailable(true)
            }
            onTouch: {
                if (event.isMove()){
                    positionSource.shouldUpdateMap = false;
                    mainPage.addAction(locateMeAction, ActionBarPlacement.Signature);
                }
                if (event.isUp()){      
                    pinContainer.updateMapWithProperClusters();
                }
            }
            
            MapView {
                id: mapview
                altitude: 2000
                latitude: 52.521386
                longitude: 13.411460
                property double lastLatitude: 0.0
                property double lastLongitude: 0.0
                property double lastAltitude: 2000
                property bool updateClusterRequired: false
                
                onRequestRender: {
                    pinContainer.updateMarkers();
                    meContainer.updateMarkers();
                }
                onAltitudeChanged: {
                    console.log(newAlt)
                    if (newAlt / lastAltitude > 1.3 || lastAltitude / newAlt > 1.3) {
                        updateClusterRequired = true;
                        lastAltitude = newAlt;
                        parent.parent.disableReservation();
                        _cityController.setAltitude(newAlt);
                    }
                }
                onLatitudeChanged: {
                    console.log(newLat - lastLatitude)
                    if (Math.abs(newLat - lastLatitude) > 0.0025) { // TODO: make it dependent on altitude
                        lastLatitude = newLat;
                        updateClusterRequired = true;
                        userControls.setStatus(newLat)
                        _cityController.setLatitude(newLat);
                    }
                }
                onLongitudeChanged: {
                    console.log(newLon - lastLongitude)
                    if (Math.abs(newLon - lastLongitude) > 0.0045) { // TODO: make it dependent on altitude
                        lastLongitude = newLon;
                        updateClusterRequired = true;
                        userControls.setStatus(newLon)
                        _cityController.setLongitude(newLon);
                    }
                }
                onCreationCompleted: {
                    setRenderEngine("RenderEngine3D")
                }
                layoutProperties: AbsoluteLayoutProperties {}
            }
            Container {
                id: meContainer
                property variant me
                overlapTouchPolicy: OverlapTouchPolicy.Allow
                layout: AbsoluteLayout {
                
                }
                function updateMarkers() {
                    _mapViewTest.updateMarkers(mapview, meContainer);
                }
                function showMe() {
                    var marker = pin.createObject();
                    marker.pinImageSource = "asset:///images/me.png";
                    marker.pointerOffsetX = ui.sdu(3.75);
                    marker.pointerOffsetY = ui.sdu(3.75);
                    insert(-1, marker);
                    marker.me = true;
                    me = marker;
                }
                function showBubble(pin) { // nothing to do here
                }
            }
            Container {
                id: pinContainer
                objectName: "pinContainer"
                
                // Must match the mapview width and height and position
                
                overlapTouchPolicy: OverlapTouchPolicy.Allow
                property bool reservationIsOngoing: false
                property variant currentBubble
                property variant lastFocusPin
                property variant reservedPin
                clipContentToBounds: true
                layout: AbsoluteLayout {
                
                }
                function reservedOk() {
                    console.log("Reservation OK")
                    if (reservedPin) {
                        reservedPin.reserve();
                    } else {
                        mapview.updateClusterRequired = true;
                        updateMapWithProperClusters(); // Needed to reload the picture of reserved car when view changed in the meantime
                    }
                    reservationIsOngoing = true;
                    userControls.reservationStarted()
                    activeFrame.updateTitleLabel("expires in");
                }
                function canceledOk() {
                    console.log("Reservation Cancel OK")
                    if (reservedPin) {
                        reservedPin.freeUp();
                    } else {
                        mapview.updateClusterRequired = true;
                        updateMapWithProperClusters(); // Needed to reload the picture of reserved car when view changed in the meantime
                    }
                    reservationIsOngoing = false;
                    userControls.resetTimeLeft();
                    userControls.reservationCancelledSucesfully()
                    activeFrame.updateTimeLeft("");
                }
                function reservedNok() {
                    console.log("Reservation NOT OK")
                    reservedPin = null;
                    userControls.reservationDeclined()
                    reservationProblem.show();
                }
                function canceledNok() {
                    console.log("Reservation Cancel NOT OK")
                    userControls.reservationCancellationDeclined()
                }
                function loggedIn(result) {
                    mainPage.userLoggedIn = true;
                    if (result) {
                        removeAllPins();
                        userControls.makeLoginControllsVisible(false)
                        userControls.setRegisterLoginAvailable(false)
                        mainPage.addAction(logoutAction, ActionBarPlacement.InOverflow);
                        if (lastFocusPin != null){
                            parent.parent.enableReservation()
                        }
                    } else {
                        userControls.makeLoginControllsVisible(true)
                    }
                }
                function loggedOut(result) {
                    if (result) {
                        //show loginRegister UI
                        mainPage.userLoggedIn = false;
                        mainPage.removeAction(logoutAction)
                        userControls.makeLoginControllsVisible(false)
                        userControls.setRegisterLoginAvailable(true)
                        userControls.makeReservationVisible(false)
           
                    } else {
                        console.log("Do nothing on NOK logout")
                    }
                    logoutAction.enabled = true;
                }
                function stopLoadingCars(){
                    userControls.stopGettingCars()
                }
                function startLoadingCars(){
                    userControls.startGettingCars()
                }                
                function showToast(message) {
                    userControls.setStatus(message)
                }
                function showToastNotification(message) {
                    generalToast.body = message;
                    generalToast.show();
                }
                function addFromModel(text, lat, lon, id, selected, pointsBelow) {
                    // if this is a debug pins dont' enable controls
                    if (selected != "DEBUG"){
//                        userControls.enableInitialLogin()
                        if (!mainPage.userLoggedIn){
                            //show loginRegister UI
                            userControls.makeLoginControllsVisible(false)
                            userControls.setRegisterLoginAvailable(true)
                        }
                    }
                    var marker = pin.createObject();
                    marker.lat = lat;
                    marker.lon = lon;
                    marker.description = text
                    marker.carId = id;
                    marker.reservState = selected;
                    if (selected){
                        console.log("Selected = " + selected)
                        console.log("Adding reserved car, text=" + text)
                    }
                    marker.pointsBelow = pointsBelow;
                    var xy = _mapViewTest.worldToPixelInvokable(mapview, marker.lat, marker.lon);
                    marker.x = xy[0];
                    marker.y = xy[1];
                    pinContainer.add(marker);
                    marker.animDrop.play();
                }
                function addPin(lat, lon) {
                    var marker = pin.createObject();
                    marker.lat = lat;
                    marker.lon = lon;
                    var xy = _mapViewTest.worldToPixelInvokable(mapview, marker.lat, marker.lon);
                    marker.x = xy[0];
                    marker.y = xy[1];
                    pinContainer.add(marker);
                    marker.animDrop.play();
                }
                function showBubble(pin) {
                    if (pin.me) {
                        return;
                    }
                    pinContainer.remove(currentBubble);
                    var details = bubble.createObject();
                    details.lat = pin.lat;
                    details.lon = pin.lon;
                    details.textToShow = pin.description;
                    var xy = _mapViewTest.worldToPixelInvokable(mapview, details.lat, details.lon);
                    details.x = xy[0];
                    details.y = xy[1];
                    pinContainer.add(details);
                    details.play();
                    currentBubble = details;
                    parent.parent.enableReservation();
                    if (lastFocusPin) {
                        lastFocusPin.unselect();
                    }
                    lastFocusPin = pin;
                }
                function updateMarkers() {
                    _mapViewTest.updateMarkers(mapview, pinContainer);
                }
                function removeBubble() {
                    pinContainer.remove(currentBubble);
                    parent.parent.disableReservation();
                    if (lastFocusPin) {
                        lastFocusPin.unselect();
                    }
                }
                function removeAllPins() {
                    parent.parent.disableReservation();
                    pinContainer.removeAll();
                }
                function updateMapWithProperClusters() {
                    if (mapview.updateClusterRequired) {
                        mapview.updateClusterRequired = false;
                        removeAllPins();
                        console.log("main.qml - UPDATING CLUSTERS")
                        _mapViewTest.adjustClusterToAltitude(mapview.altitude, mapview.latitude, mapview.longitude);
                    }
                }
                layoutProperties: AbsoluteLayoutProperties {}
            }
            UserControls{
                id: userControls
                onReservationRequested:{
                    pinContainer.reservedPin = pinContainer.lastFocusPin; // remember which to reserve
                }
            }
        }
        function onThumbnail() {
            activeFrame.updateTimeLeft("");
        }
        
        function enableReservation(){
            if (!pinContainer.reservedPin && userLoggedIn){
                userControls.makeReservationVisible(true);
            }
            if (!userLoggedIn){
                userControls.playBounceAnimation()
            }
        }
        
        function disableReservation(){
            if (!pinContainer.reservedPin){
                if (!pinContainer.reservationIsOngoing){
                    userControls.makeReservationVisible(false);
                }
            }
        }
        
        attachedObjects: [
            ActionItem {
                id: locateMeAction
                title: "Locate me"
                backgroundColor: Color.create("#aa007ea4")
                imageSource: "asset:///images/locateMe.png"
                onTriggered: {
                    positionSource.shouldUpdateMap = true;
                    mainPage.removeAction(locateMeAction)
                }
            },
            ActionItem {
                id: logoutAction
                ActionBar.placement: ActionBarPlacement.InOverflow
                imageSource: "asset:///images/logout.png"
                title: "Logout"
                onTriggered: {
                    if (pinContainer.reservedPin){
                        pinContainer.reservedPin.freeUp();
                    }
                    userControls.cleanUp()
                    userData.clean();
                    _mapViewTest.logout()
                    logoutAction.enabled = false
                }
            },
            ComponentDefinition {
                id: pin
                source: "pin.qml"
            },
            ComponentDefinition {
                id: bubble
                source: "bubble.qml"
            },
            ComponentDefinition {
                id: cityPicker
                source: "CityPicker.qml"
            },
            ComponentDefinition {
                id: voteView
                source: "VoteView.qml"
            },
            ComponentDefinition {
                id: listOfCars
                source: "ListOfCars.qml"
            },
            SystemToast {
                id: reservationProblem
                body: "Problem with reservation, make sure the car is not booked already."
            },
            SystemToast {
                id: cancellationProblem
                body: "Was not able to cancel your reservation. Try again in a minute."
            },
            SystemToast {
                id: generalToast
                body: ""
                button.label: "Ok"
                button.enabled: true
            },
            SystemToast {
                id: reservationExpired
                body: "Your reservation has expired."
                button.label: "Got it"
                button.enabled: true
                
            },
            Compass {
                property double azimuth: 0
                active: true
                axesOrientationMode: Compass.UserOrientation
                alwaysOn: false
                onReadingChanged: { // Called when a new compass reading is available
                    if (meContainer.me){
                        meContainer.me.rotationZ = 360 + reading.azimuth;
                    }
                }
            },
            PositionSource {
                id: positionSource
                updateInterval: 1000
                active: true
                property bool shouldUpdateMap: true
                onPositionChanged: {                    
                    console.log("onPositionChanged")
                    if (shouldUpdateMap){
                        console.log("updateMap")
                        mapview.latitude = positionSource.position.coordinate.latitude;
                        mapview.longitude = positionSource.position.coordinate.longitude;
                        pinContainer.updateMapWithProperClusters();
                    }
                    // update user position on the map
                    meContainer.me.lat = positionSource.position.coordinate.latitude;
                    meContainer.me.lon = positionSource.position.coordinate.longitude;
                    // Update user coordinate to calculate distance from cars
                    _mapViewTest.updateUserLatitude(positionSource.position.coordinate.latitude);
                    _mapViewTest.updateUserLongitude(positionSource.position.coordinate.longitude);
                    var xy = _mapViewTest.worldToPixelInvokable(mapview, meContainer.me.lat, meContainer.me.lon);
                    meContainer.me.x = xy[0];
                    meContainer.me.y = xy[1];
                }
            }
        ]
    }
    
    function setMapToLookAt(lat, lon, alt){
        mainPage.disableReservation();
        positionSource.shouldUpdateMap = false;
        mainPage.addAction(locateMeAction, ActionBarPlacement.Signature);
        mapview.setLatitude(lat)
        mapview.setLongitude(lon)
        mapview.setAltitude(alt)
    }
}