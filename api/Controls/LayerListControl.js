
LayerListControl = function(element_id, map) {
  this.id = element_id;
  this.element = jQuery("#" + element_id);
  this.map = map;
  this.init();
};

LayerListControl.prototype = {
  init: function() {

    this.updateList();


    //TODO: Add iframe shim to allow control to float over plugin control?
    //var height = jQuery(this.element).height();
    //var width = jQuery(this.element).width();
    //jQuery(this.element).prepend('<iframe id="iframe_shim" style="clear:both; position: absolute; width:' + width + '; height:' + height + '; z-index:-100; background: white"></iframe>');


    //TODO: Smarter updating instead of redrawing the entire list with each change
    var thisObj = this;
    this.map.addEvent("imagelayeradded", function(data) { thisObj.updateList(); });
    this.map.addEvent("imagelayerremoved", function(data) { thisObj.updateList(); });
    this.map.addEvent("imagelayermoved", function(data) { thisObj.updateList(); });
  },

  updateList: function() {

    //Remove all existing children
    jQuery(this.element).children().remove();

    //jQuery(this.element).append('<div class="ui-widget-header"><span id="layer-header-toggle" style="float:left" class="ui-icon ui-icon-triangle-1-s"></span>Layers</div>');
    jQuery(this.element).append('<div class="ui-widget-header">Layers</div>');

    var layerContainer = jQuery('<div id="layer_container"/>');
    jQuery(this.element).append(layerContainer);

    jQuery("#layer-header-toggle").bind("click", function() {
      jQuery("#layer_container").slideToggle();
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
        var div = jQuery('<div id="layer_' + layers.ids[i] + '">')
                        .addClass('ui-widget-content ui-state-default ui-helper-clearfix');

        var props = this.map.getImageLayerProperties(layers.ids[i]);

        jQuery(div).append('<div class="drag_handle"><input id="layercheck_' + i + '" type="checkbox" checked="checked"/><span>' + layers.names[i] + '</span></div>');
        jQuery(div).append('<div id="layeropacity_' + i + '" class="opacity-slider"></div>');

        jQuery(layerContainer).append(div);

        jQuery("#layercheck_" + i).bind("click", { layerId: layers.ids[i], map: this.map }, function(event) {
          var checked = jQuery(this).attr("checked");

          var layerProps = new ImageLayerProperties(event.data.layerId);
          layerProps.setVisible(checked);
          event.data.map.updateImageLayer(layerProps);
        });

        jQuery(layerContainer).sortable({
          handle: '.drag_handle',
          cursor: 'move',
          map: this.map,
          update: function(event, ui) {
            jQuery(this).data('sortable').options.map.moveImageLayer(new ImageLayerProperties(ui.item.attr('id').split("_").pop()), ui.item.index());
            //alert(ui.item.attr('id').split("_").pop() + 'at' + ui.item.index());
          }
        });
        jQuery(layerContainer).disableSelection();

        var opacity = props.opacity;
        jQuery('#layeropacity_' + i).slider({
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

  }
};