/* Copyright (c) 2012, 2013  BlackBerry Limited.
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

//! [0]
Container {
    // File path of the pin image
    property string pinImageSource: qsTr("asset:///images/on_map_pin.png")
    property string pinImageSelected: qsTr("asset:///images/on_map_pinSelected.png")
    property string pinImageBB: qsTr("asset:///images/BbLogo.png")
    property string pinImageReserved :qsTr("asset:///images/on_map_pinR.png")
    // pointerOffsetX, pointerOffsetY is the position of the pixel in pin image that should point to the location. Change these to match your pin image.
    property int pointerOffsetX: ui.sdu(2.5)
    property int pointerOffsetY: ui.sdu(7.25)
    /////////////////////////////////////////////////////////
    id: root
    property int x: 0
    property int y: 0
    property double lat
    property double lon
    property string description
    property string carId
    property bool reservState: false
    property bool me: false
    property int pointsBelow: 0
    property int touchCount: 0
    property alias animDrop: animDrop
    clipContentToBounds: false
    overlapTouchPolicy: OverlapTouchPolicy.Allow
    layoutProperties: AbsoluteLayoutProperties {
        id: position
        positionX: x - pointerOffsetX
        positionY: y - pointerOffsetY
    }
    function unselect(){
        if (reservState == true){
            return;
        }
        if (pointsBelow == 0){
            pinImage.imageSource = pinImageSource;
        }
    }
    function reserve(){
        reservState = true;
        pinImage.imageSource = pinImageReserved;
    }
    function freeUp(){
        reservState = false;
        pinImage.imageSource = pinImageSource;
        animRise.play();
    }
    animations: [
        TranslateTransition {
            id: animDrop
        },
        ScaleTransition {            
            id: animRise
            fromX: 0.01
            toX: 1
            fromY: 0.01
            toY: 1
            duration: 400
            easingCurve: StockCurve.CubicIn
        }
    ]
    attachedObjects: [
        ImplicitAnimationController {
            // Disable animations to avoid jumps between 0 and 360 degree
            enabled: false
        }
    ]
    layout: DockLayout {
    }
    ImageView {
        id: pinImage
        scaleX: .8
        scaleY: .8
        imageSource: reservState ? pinImageReserved : pinImageSource
        focusPolicy: FocusPolicy.Touch
        overlapTouchPolicy: OverlapTouchPolicy.Allow
        onFocusedChanged: {
            if (focused) {
                animFocus.play();
                animLblFocus.play();
                if (pointsBelow == 0){
                    root.parent.showBubble(root);
                    if (reservState){
                        return;
                    }
                    if (!me){
                        imageSource = pinImageSelected;
                    }
                }
                touchCount++;
                if (touchCount > 30){
                    imageSource = pinImageBB;
                }
            }
            if (!focused) {
                animUnfocus.play();
                animLblFocus.play();
            }
        }
        animations: [
            ScaleTransition {
                id: animFocus
                fromX: .8
                toX: 1
                fromY: .8
                toY: 1
                duration: 300
                easingCurve: StockCurve.BounceOut
            },
            ScaleTransition {
                id: animUnfocus
                fromX: 1
                toX: .8
                fromY: 1
                toY: .8
                duration: 300
                easingCurve: StockCurve.BounceOut
            }
        ]
    }
    Container {
        visible: pointsBelow ? true : false
        horizontalAlignment: HorizontalAlignment.Right
        verticalAlignment: VerticalAlignment.Top
        overlapTouchPolicy: OverlapTouchPolicy.Allow
        layout: DockLayout {
        }
        Label {
            id: points
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Top
            overlapTouchPolicy: OverlapTouchPolicy.Allow
            text: pointsBelow ? pointsBelow.toString() : ""
            textStyle.color: Color.Black
            textStyle.fontSize: FontSize.XXSmall
            textStyle.fontWeight: FontWeight.Bold
            animations: [
                ScaleTransition {
                    id: animLblFocus
                    fromX: .8
                    toX: 1
                    fromY: .8
                    toY: 1
                    duration: 300
                    easingCurve: StockCurve.BounceOut
                },
                ScaleTransition {
                    id: animLblUnfocus
                    fromX: 1
                    toX: .8
                    fromY: 1
                    toY: .8
                    duration: 300
                    easingCurve: StockCurve.BounceOut
                }
            ]
        }        
    }
}
//! [0]
