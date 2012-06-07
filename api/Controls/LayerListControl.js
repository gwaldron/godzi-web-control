
LayerListControl = function(element_id, map) {
  this.id = element_id;
  this.element = jQuery("#" + element_id);
  this.map = map;
  this.init();
};

LayerListControl.prototype = {
  init: function() {

    jQuery(this.element).append('<div id="imgageGroupParent" style="margin-bottom: 20px;"/>');
    jQuery(this.element).append('<div id="elevGroupParent" style="margin-bottom: 20px;"/>');
    jQuery(this.element).append('<div id="modelGroupParent" style="margin-bottom: 20px;"/>');
    jQuery(this.element).append('<div id="annoGroupParent"/>');

    this.updateImageList();
    this.updateElevList();
    this.updateModelList();
    this.initAnnotationList();


    //TODO: Add iframe shim to allow control to float over plugin control?
    //var height = jQuery(this.element).height();
    //var width = jQuery(this.element).width();
    //jQuery(this.element).prepend('<iframe id="iframe_shim" style="clear:both; position: absolute; width:' + width + '; height:' + height + '; z-index:-100; background: white"></iframe>');


    //TODO: Smarter updating instead of redrawing the entire list with each change
    var thisObj = this;
    this.map.addEvent("imagelayeradded", function(data) { thisObj.updateImageList(); });
    this.map.addEvent("imagelayerremoved", function(data) { thisObj.updateImageList(); });
    this.map.addEvent("imagelayermoved", function(data) { thisObj.updateImageList(); });
    this.map.addEvent("elevationlayeradded", function(data) { thisObj.updateElevList(); });
    this.map.addEvent("elevationlayerremoved", function(data) { thisObj.updateElevList(); });
    this.map.addEvent("elevationlayermoved", function(data) { thisObj.updateElevList(); });
    this.map.addEvent("modellayeradded", function(data) { thisObj.updateModelList(); });
    this.map.addEvent("modellayerremoved", function(data) { thisObj.updateModelList(); });
    this.map.addEvent("modellayermoved", function(data) { thisObj.updateModelList(); });
  },

  showImageLayers: function(visible) {
    if (visible)
      jQuery("#imgageGroupParent").show();
    else
      jQuery("#imgageGroupParent").hide();
  },

  showElevationLayers: function(visible) {
    if (visible)
      jQuery("#elevGroupParent").show();
    else
      jQuery("#elevGroupParent").hide();
  },

  showModelLayers: function(visible) {
    if (visible)
      jQuery("#modelGroupParent").show();
    else
      jQuery("#modelGroupParent").hide();
  },

  showAnnotations: function(visible) {
    if (visible)
      jQuery("#annoGroupParent").show();
    else
      jQuery("#annoGroupParent").hide();
  },

  updateImageList: function() {

    var imageParent = jQuery("#imgageGroupParent");

    //Remove all existing children
    jQuery(imageParent).children().remove();

    jQuery(imageParent).append('<div class="ui-widget-header"><span id="imagelayer-header-toggle" style="float:left" class="ui-icon ui-icon-triangle-1-s"></span>Image Layers</div>');

    var layerContainer = jQuery('<div id="imagelayer_container"/>');
    jQuery(imageParent).append(layerContainer);

    jQuery("#imagelayer-header-toggle").bind("click", function() {
      jQuery("#imagelayer_container").slideToggle();
      var l = jQuery(this);
      if (l.hasClass("ui-icon-triangle-1-s")) {
        l.removeClass("ui-icon-triangle-1-s");
        l.addClass("ui-icon-triangle-1-e");
      }
      else if (l.hasClass('ui-icon-triangle-1-e')) {
        l.addClass("ui-icon-triangle-1-s");
        l.removeClass("ui-icon-triangle-1-e");
      }
    });

    var layers = this.map.getImageLayers();

    for (var i = 0; i < layers.ids.length; i++) {
      if (layers.ids[i].length > 0) {
        var div = jQuery('<div id="imagelayer_' + layers.ids[i] + '" style="padding: 0px 10px 10px 10px;">')
                        .addClass('ui-widget-content ui-state-default ui-helper-clearfix');

        var props = this.map.getImageLayerProperties(layers.ids[i]);

        jQuery(div).append('<div class="drag_handle"><input id="imagelayercheck_' + i + '" type="checkbox" checked="checked"/><span>' + layers.names[i] + '</span></div>');
        jQuery(div).append('<div id="imagelayeropacity_' + i + '" class="opacity-slider"></div>');

        jQuery(layerContainer).append(div);

        jQuery("#imagelayercheck_" + i).bind("click", { layerId: layers.ids[i], map: this.map }, function(event) {
          var checked = jQuery(this).prop("checked");

          var layerProps = new ImageLayerProperties(event.data.layerId);
          layerProps.setVisible(checked);
          event.data.map.updateImageLayer(layerProps);
        });

        jQuery(layerContainer).sortable({
          handle: '.drag_handle',
          cursor: 'move',
          map: this.map,
          update: function(event, ui) {
            jQuery(this).data('sortable').options.map.moveImageLayer(new ImageLayerProperties(ui.item.prop('id').split("_").pop()), ui.item.index());
            //alert(ui.item.prop('id').split("_").pop() + 'at' + ui.item.index());
          }
        });
        jQuery(layerContainer).disableSelection();

        var opacity = props.opacity;
        jQuery('#imagelayeropacity_' + i).slider({
          min: 0,
          max: 100,
          value: opacity * 100.0,
          range: "min",
          map: this.map,
          layerId: layers.ids[i],
          slide: function(event, ui) {
            var opacity = ui.value / 100.0;
            var layerProps = new ImageLayerProperties(jQuery(this).data('slider').options.layerId);
            layerProps.setOpacity(opacity);
            jQuery(this).data('slider').options.map.updateImageLayer(layerProps);
          }
        });
      }
    }
  },

  updateElevList: function() {

    var elevParent = jQuery("#elevGroupParent");

    //Remove all existing children
    jQuery(elevParent).children().remove();

    jQuery(elevParent).append('<div class="ui-widget-header"><span id="elevlayer-header-toggle" style="float:left" class="ui-icon ui-icon-triangle-1-s"></span>Elevation Layers</div>');

    var layerContainer = jQuery('<div id="elevlayer_container"/>');
    jQuery(elevParent).append(layerContainer);

    jQuery("#elevlayer-header-toggle").bind("click", function() {
      jQuery("#elevlayer_container").slideToggle();
      var l = jQuery(this);
      if (l.hasClass("ui-icon-triangle-1-s")) {
        l.removeClass("ui-icon-triangle-1-s");
        l.addClass("ui-icon-triangle-1-e");
      }
      else if (l.hasClass('ui-icon-triangle-1-e')) {
        l.addClass("ui-icon-triangle-1-s");
        l.removeClass("ui-icon-triangle-1-e");
      }
    });

    var layers = this.map.getElevationLayers();

    for (var i = 0; i < layers.ids.length; i++) {
      if (layers.ids[i].length > 0) {
        var div = jQuery('<div id="elevlayer_' + layers.ids[i] + '" style="padding: 0px 10px 0px 10px;">')
                        .addClass('ui-widget-content ui-state-default ui-helper-clearfix');

        jQuery(div).append('<div class="drag_handle"><input id="elevlayercheck_' + i + '" type="checkbox" checked="checked"/><span>' + layers.names[i] + '</span></div>');
        //jQuery(div).append('<div id="elevlayeropacity_' + i + '" class="opacity-slider"></div>');

        jQuery(layerContainer).append(div);

        jQuery("#elevlayercheck_" + i).bind("click", { layerId: layers.ids[i], map: this.map }, function(event) {
          var checked = jQuery(this).prop("checked");
          event.data.map.toggleElevationLayer({ "_id": event.data.layerId }, checked);
        });

        //        var opacity = props.opacity;
        //        jQuery('#elevlayeropacity_' + i).slider({
        //          min: 0,
        //          max: 100,
        //          value: opacity * 100.0,
        //          range: "min",
        //          map: this.map,
        //          layerId: layers.ids[i],
        //          slide: function(event, ui) {
        //            var opacity = ui.value / 100.0;
        //            var layerProps = new ImageLayerProperties(jQuery(this).data('slider').options.layerId);
        //            layerProps.setOpacity(opacity);
        //            jQuery(this).data('slider').options.map.updateImageLayer(layerProps);
        //          }
        //        });
      }
    }

    jQuery(layerContainer).sortable({
      handle: '.drag_handle',
      cursor: 'move',
      map: this.map,
      update: function(event, ui) {
        jQuery(this).data('sortable').options.map.moveElevationLayer({ "_id": ui.item.prop('id').split("_").pop() }, ui.item.index());
      }
    });

    jQuery(layerContainer).disableSelection();
  },

  updateModelList: function() {

    var modelParent = jQuery("#modelGroupParent");

    //Remove all existing children
    jQuery(modelParent).children().remove();

    jQuery(modelParent).append('<div class="ui-widget-header"><span id="modellayer-header-toggle" style="float:left" class="ui-icon ui-icon-triangle-1-s"></span>Model Layers</div>');

    var layerContainer = jQuery('<div id="modellayer_container"/>');
    jQuery(modelParent).append(layerContainer);

    jQuery("#modellayer-header-toggle").bind("click", function() {
      jQuery("#modellayer_container").slideToggle();
      var l = jQuery(this);
      if (l.hasClass("ui-icon-triangle-1-s")) {
        l.removeClass("ui-icon-triangle-1-s");
        l.addClass("ui-icon-triangle-1-e");
      }
      else if (l.hasClass('ui-icon-triangle-1-e')) {
        l.addClass("ui-icon-triangle-1-s");
        l.removeClass("ui-icon-triangle-1-e");
      }
    });

    var layers = this.map.getModelLayers();

    for (var i = 0; i < layers.ids.length; i++) {
      if (layers.ids[i].length > 0) {
        var div = jQuery('<div id="modellayer_' + layers.ids[i] + '" style="padding: 0px 10px 0px 10px;">')
                        .addClass('ui-widget-content ui-state-default ui-helper-clearfix');

        jQuery(div).append('<div class="drag_handle" style="position: relative; padding-bottom: 2px;"><span><input id="modellayercheck_' + i + '" type="checkbox" checked="checked"/>' + layers.names[i] + '</span><button id="modellayerbutton_' + i + '"></button></div>');
        //jQuery(div).append('<div id="modellayeropacity_' + i + '" class="opacity-slider"></div>');

        jQuery(layerContainer).append(div);

        jQuery("#modellayercheck_" + i).bind("click", { layerId: layers.ids[i], map: this.map }, function(event) {
          var checked = jQuery(this).prop("checked");
          event.data.map.toggleModelLayer({ "_id": event.data.layerId }, checked);
        });


        jQuery("#modellayerbutton_" + i).button({ icons: { primary: "ui-icon-search" }, text: false });

        jQuery("#modellayerbutton_" + i).css({ 'position': 'absolute', 'right': '0px', 'padding': '4px 2px', 'font-size': '50%', 'margin-top': '2px' });

        jQuery("#modellayerbutton_" + i).bind("click", { layerId: layers.ids[i], map: this.map }, function(event) {
          var bounds = event.data.map.getModelLayerBounds({ "_id": event.data.layerId });
          event.data.map.setViewpoint({ latitude: bounds.latitude, longitude: bounds.longitude, pitch: -90, range: bounds.radius * 4.0 }, 1);
        });


        //        var opacity = props.opacity;
        //        jQuery('#modellayeropacity_' + i).slider({
        //          min: 0,
        //          max: 100,
        //          value: opacity * 100.0,
        //          range: "min",
        //          map: this.map,
        //          layerId: layers.ids[i],
        //          slide: function(event, ui) {
        //            var opacity = ui.value / 100.0;
        //            var layerProps = new ImageLayerProperties(jQuery(this).data('slider').options.layerId);
        //            layerProps.setOpacity(opacity);
        //            jQuery(this).data('slider').options.map.updateImageLayer(layerProps);
        //          }
        //        });
      }
    }

    jQuery(layerContainer).sortable({
      handle: '.drag_handle',
      cursor: 'move',
      map: this.map,
      update: function(event, ui) {
        jQuery(this).data('sortable').options.map.moveModelLayer({ "_id": ui.item.prop('id').split("_").pop() }, ui.item.index());
      }
    });

    jQuery(layerContainer).disableSelection();
  },

  initAnnotationList: function() {

    var annoParent = jQuery("#annoGroupParent");

    //Remove all existing children
    jQuery(annoParent).children().remove();

    jQuery(annoParent).append('<div class="ui-widget-header"><span id="annolayer-header-toggle" style="float:left" class="ui-icon ui-icon-triangle-1-s"></span>Annotations</div>');

    var layerContainer = jQuery('<div id="annolayer_container"/>');
    jQuery(annoParent).append(layerContainer);

    jQuery("#annolayer-header-toggle").bind("click", function() {
      jQuery("#annolayer_container").slideToggle();
      var l = jQuery(this);
      if (l.hasClass("ui-icon-triangle-1-s")) {
        l.removeClass("ui-icon-triangle-1-s");
        l.addClass("ui-icon-triangle-1-e");
      }
      else if (l.hasClass('ui-icon-triangle-1-e')) {
        l.addClass("ui-icon-triangle-1-s");
        l.removeClass("ui-icon-triangle-1-e");
      }
    });

    //    jQuery(layerContainer).sortable({
    //      handle: '.drag_handle',
    //      cursor: 'move',
    //      map: this.map,
    //      update: function(event, ui) {
    //        jQuery(this).data('sortable').options.map.moveAnnotationLayer({ "_id": ui.item.prop('id').split("_").pop() }, ui.item.index());
    //      }
    //    });

    jQuery(layerContainer).disableSelection();
  },

  addAnnotation: function(annotation) {
    if (annotation != undefined) {
      var id = annotation.getId();
      var div = jQuery('<div id="annolayer_' + id + '" style="padding: 0px 10px 0px 10px;">')
                        .addClass('ui-widget-content ui-state-default ui-helper-clearfix');

      jQuery(div).append('<div class="drag_handle" style="position: relative; padding-bottom: 2px;"><span><input id="annolayercheck_' + id + '" type="checkbox" checked="checked"/>' + annotation.getName() + '</span><button id="annolayerbutton_' + id + '"></button></div>');

      var layerContainer = jQuery('#annolayer_container');
      jQuery(layerContainer).append(div);

      jQuery("#annolayercheck_" + id).bind("click", { "annotation": annotation }, function(event) {
        event.data.annotation.setVisible(jQuery(this).prop("checked"));
      });

      var colors = this.map.getAnnotationColors(id);

      if (colors != undefined && colors.fill != undefined && colors.fill != "") {
        // add color picker
        var input = $('<input type="hidden" id="fillcolorpicker_' + id + '" class="color-picker" size="6" value="' + colors.fill + '" />');
        input.insertBefore($("#annolayerbutton_" + id));

        // add div for opacity slider
        jQuery(div).append('<div id="annolayeropacity_' + id + '" class="opacity-slider"></div>');
        jQuery(div).css("padding", "0px 10px 10px 10px");

        // setup fill color picker
        $("#fillcolorpicker_" + id).miniColors({
          letterCase: 'uppercase',
          change: function(hex, rgb) {
            annotation.setFill(hex);
          }
        });

        // setup opacity slider
        var opacity = 1.0;
        if (colors.opacity != undefined)
          opacity = colors.opacity;

        jQuery('#annolayeropacity_' + id).slider({
          min: 0,
          max: 100,
          value: opacity * 100.0,
          range: "min",
          map: this.map,
          annoId: id,
          slide: function(event, ui) {
            var opacity = Math.round(ui.value / 100.0 * 255.0);
            var s = opacity.toString(16);
            
            //Pad with leading zeros if necessary
            if (s.length < 2)
              s = ("00" + s).slice(-2);
              
            annotation.setOpacity(s);
          }
        });
      }

      if (colors != undefined && colors.stroke != undefined && colors.stroke != "") {
        var input = $('<input type="hidden" id="strokecolorpicker_' + id + '" class="color-picker" size="6" value="' + colors.stroke + '" />');
        input.insertBefore($("#annolayerbutton_" + id));

        $("#strokecolorpicker_" + id).miniColors({
          letterCase: 'uppercase',
          change: function(hex, rgb) {
            annotation.setStroke(hex);
          }
        });
      }

      jQuery("#annolayerbutton_" + id).button({ icons: { primary: "ui-icon-search" }, text: false });

      jQuery("#annolayerbutton_" + id).css({ 'position': 'absolute', 'right': '0px', 'padding' : '4px 2px', 'font-size': '50%', 'margin-top': '2px' });

      jQuery("#annolayerbutton_" + id).bind("click", { "annotation": annotation, map: this.map }, function(event) {
        var bounds = event.data.annotation.getBounds();
        event.data.map.setViewpoint({ latitude: bounds.latitude, longitude: bounds.longitude, pitch: -90, range: bounds.radius * 4.0 }, 1);
      });
    }
  },

  removeAnnotation: function(annotation) {
    if (annotation != undefined) {
      var div = jQuery('<div id="annolayer_' + annotation.getId() + '">').remove();
    }
  }

};