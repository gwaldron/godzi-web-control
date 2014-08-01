TerrainProfileButtonControl = function(element_id, map, callback) {
  this.id = element_id;
  this.element = jQuery("#" + element_id);
  this.map = map;
  this.callback = callback;
  this.init();
};

TerrainProfileButtonControl.prototype = {
  init: function() {

    this.capturing = false;
    this.point1 = null;
    this.place1 = null;
    this.place2 = null;
    this.profileLine = null;

    this.$button = jQuery('<button class="terrain-profile-button">Terrain Profile</button>');
    jQuery(this.element).append(this.$button);

    this.$button.click({ context: this }, this.onButtonClick);

    this.map.addEvent("onclick", this.onMapClick, this);
  },

  onButtonClick: function(event) {
    event.data.context.setCapturing(!event.data.context.capturing);
  },

  clearDisplay: function() {
    if (this.place1) {
      this.map.sendCommand("removeAnnotationNode", { id: this.place1.getId() }, false);
    }

    if (this.place2) {
      this.map.sendCommand("removeAnnotationNode", { id: this.place2.getId() }, false);
    }

    if (this.profileLine) {
      this.map.sendCommand("removeAnnotationNode", { id: this.profileLine.getId() }, false);
    }
  },

  setCapturing: function(isCapturing) {
    if (isCapturing) {
      this.clearDisplay();
      this.capturing = true;
      this.$button.css("background-color", "#6ff");
    }
    else {
      this.capturing = false;
      this.point1 = null;
      this.$button.css("background-color", "");
    }
  },

  onMapClick: function(click, context) {

    if (context.capturing) {
      if (context.point1) {

        // Add end marker and line
        var loc = context.map.getIntersection(click.x, click.y);
        if (loc) {
          loc.setAltitude(0.0);
          context.place2 = new PlaceAnnotation({
            text: "",
            location: loc,
            icon: "http://s3.amazonaws.com/pelican-downloads/aam/data/placemark32.png",
            style: { text_size: "28" }
          });
          context.place2.setMap(context.map);

          var loc1 = context.map.getIntersection(context.point1.x, context.point1.y);
          if (loc1) {
            loc1.setAltitude(0.0);
            context.profileLine = new FeatureAnnotation({
              geomWKT: "LINESTRING(" + loc1.getLongitude() + " " + loc1.getLatitude() + ", " + loc.getLongitude() + " " + loc.getLatitude() + " )",
              style: {
                stroke: "#ffff00",
                stroke_width: "5"
              },
              draped: true
            });
            context.profileLine.setMap(context.map);
          }
        }

        var profile = context.map.getTerrainProfile(context.point1.x, context.point1.y, click.x, click.y);
        if (profile) {
          var output = "total distance: " + profile.totalDistance.toFixed(2) + "<br />";
          output += "min/max elevation: " + profile.elevationRange.min.toFixed(2) + " / " + profile.elevationRange.max.toFixed(2) + "<br /><br />";
          output += "profile points (distance/elevation):<br />";

          for (i = 0; i < profile.profile.length; i++) {
            output += "   " + profile.profile[i].distance.toFixed(2) + " / " + profile.profile[i].elevation.toFixed(2) + "<br />";
          }

          if (context.callback) {
            context.callback(output);
          }
        }
        else {
          context.callback(null);
        }

        context.setCapturing(false);
      }
      else {
        context.point1 = click;

        var loc = context.map.getIntersection(click.x, click.y);
        if (loc) {
          loc.setAltitude(0.0);
          context.place1 = new PlaceAnnotation({
            text: "",
            location: loc,
            icon: "http://s3.amazonaws.com/pelican-downloads/aam/data/placemark32.png",
            style: { text_size: "28" }
          });
          context.place1.setMap(context.map);
        }
      }
    }

  }
}