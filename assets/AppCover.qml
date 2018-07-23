import bb.cascades 1.3

Container {
    Container {       
        layout: DockLayout {}
        background: Color.create("#28c1ee")
        
        ImageView {
            imageSource: "asset:///images/1440x1440.png"
            scalingMethod: ScalingMethod.AspectFill
        }
        
        Container {
            objectName: "timerContainer"
            visible: false
            bottomPadding: ui.du(3.875)
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Bottom
            layout: StackLayout {
                orientation: LayoutOrientation.TopToBottom
            }
            Container {
                preferredWidth: ui.du(43.75)
                preferredHeight: ui.du(5.25)
                background: Color.create("#ee7422")
                layout: DockLayout {}
                bottomMargin: ui.du(0.5)
                Label {
                    objectName: "nameLabelActiveFrame"
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    text: ""
                    textStyle.color: Color.create("#ebebeb")
                    textStyle.fontSize: FontSize.PointValue
                    textStyle.fontSizeValue: ui.du(1.25)
                }
            }
            
            Container {
                preferredWidth: ui.du(43.75)
                preferredHeight: ui.du(5.25)
                background: Color.create("#ee7422")                
                layout: DockLayout {}
                Label {
                    objectName: "statusLabelActiveFrame"
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    text: ""
                    textStyle.color: Color.create("#ebebeb")
                    textStyle.fontSize: FontSize.PointValue
                    textStyle.fontSizeValue: ui.du(1.25)
                }
            }
        }
    }
}
