import bb.cascades 1.3

Page {
    property NavigationPane nav: null
    Container {
        ListView {
            id: listView
            dataModel: dataCities
            property string lastSelectedCityCode: ""
            property double lon: 0
            property double lat: 0
            property double alt: 0
            listItemComponents: [
                ListItemComponent {
                    type: "country"
                    Header {
                        title: ListItemData.value
                    }
                },
                ListItemComponent {
                    type: "city"
                    StandardListItem {
                        title: ListItemData.name
                        //imageSource: "asset:///images/cities/" + ListItemData.picture
                    }
                }
            ]
            onTriggered: {
                console.log(indexPath)
                lastSelectedCityCode = dataModel.data(indexPath).code.toString()
                var city = dataModel.data(indexPath);
                lat = city.lat
                lon = city.lon
                alt = city.alt
                listView.clearSelection()
                select(indexPath)
                console.log(lastSelectedCityCode)
                if (lastSelectedCityCode){
                    _cityController.changeCityTo(city.code, alt, lat, lon);
                    _mapViewTest.setCurrentCity(lastSelectedCityCode)
                    nav.setMapToLookAt(lat, lon, alt)
                    nav.pop();
                }
            }
        }
        attachedObjects: [
            XmlDataModel {
                id: dataCities
                source: "data/cities.xml"
            }
        ]
    }
    function setNavigator(navigator){
        nav = navigator;   
    }
}
