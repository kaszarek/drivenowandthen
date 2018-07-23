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
import bb.system 1.0

Container {
    id: bubbleContainer
    // File path of the bubble image
    property string bubbleImageSource: "asset:///768x1280/bubble.png"
    // pointerOffsetX, pointerOffsetY is the position of the pixel in bubble image that should point to the location. Change these to match your pin image.
    property int pointerOffsetX: ui.du(16.875)
    property int pointerOffsetY: ui.du(22.5)
    // Width and Height of the pin image. Used to further offset the bubble position so that it does not overlap the pin image.
    property int pinWidth: ui.du(7.5)
    property int pinHeight: ui.du(7.5)
    // Width of the content area
    property int contentWidth: ui.du(54.875)
    // Height of the content area
    property int contentHeight: ui.du(16.625)
    // Padding of the content area i.e. the border size of the bubble.
    property int padding: ui.du(2.5)
    ////////////////////////////////////////////////////////////////////////
    property int x: 0
    property int y: 0
    property int actualX: x - pointerOffsetX
    property int actualY: y - pointerOffsetY - pinHeight
    property double lat
    property double lon
    property string textToShow
    overlapTouchPolicy: OverlapTouchPolicy.Allow
    layoutProperties: AbsoluteLayoutProperties {
        id: position
        positionX: actualX
        positionY: actualY
    }
    layout: DockLayout {
    }
    ImageView {
        imageSource: bubbleImageSource
        overlapTouchPolicy: OverlapTouchPolicy.Allow
    }
    Container {
        topPadding: padding
        leftPadding: padding
        rightPadding: padding
//        preferredHeight: contentHeight
//        preferredWidth: contentWidth
//        maxHeight: contentHeight
//        maxWidth: contentWidth
        overlapTouchPolicy: OverlapTouchPolicy.Allow
        layout: DockLayout {
        }
        Label {
            textStyle.fontSize: FontSize.Small
            horizontalAlignment: HorizontalAlignment.Left
            verticalAlignment: VerticalAlignment.Center
            overlapTouchPolicy: OverlapTouchPolicy.Allow
            text: bubbleContainer.textToShow
            textStyle.color: Color.White
            multiline: true
        }
    }
    animations: [
        ParallelAnimation {
            id: anim
            ScaleTransition {
                fromX: 0
                toX: 1
                fromY: 0
                toY: 1
                duration: 300
                easingCurve: StockCurve.BounceOut
            }
            FadeTransition {
                fromOpacity: 0
                toOpacity: 1
                duration: 200
            }
        }
    ]
    function play() {
        anim.play();
    }
}
