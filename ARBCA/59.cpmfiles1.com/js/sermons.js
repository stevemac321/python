var sermonTemplate ='{{#series}}'+
						'<div class="series-description">'+
							'{{#video}}<div class="series-video" style="margin-bottom:10px">{{> videoPlayer}}{{/video}}</div>'+
							'{{^video}}{{#image}}<div class="series-graphic"><img src="{{image}}" />{{/image}}{{/video}}'+
							'<div class="series-content">'+
								'<h4>Series: {{title}}</h3>'+
								'{{{content}}}'+
							'</div>'+
						'</div>'+
					'{{/series}}'+
					'{{#sermons}}'+
					'<div class="sermon">'+
					'{{#series_image}}<img src="{{series_image}}" alt="{{series_title}}" width="83" height="52" />{{/series_image}}'+
					'<h4><span class="date">{{date3}}</span> <a href="{{url}}">{{title}}</a></h4>'+
					'<p class="meta">'+
					'{{#speaker}}<strong>by:</strong> {{speaker}}{{/speaker}}'+
					'{{#speaker}} {{#series_title}} | {{/series_title}} {{/speaker}}'+
					'{{#series_title}}<strong>Series:</strong> {{series_title}}{{/series_title}}'+
					'{{#passage}} | <strong>Scripture:</strong> {{passage}} {{/passage}}'+
					'</p>'+
					'<ul>'+
					'{{#video}}<li class="sl_watch"><a href="{{video}}"{{#embed}}{{embed}}{{/embed}}>Watch</a></li>{{/video}}'+
					'{{#audio}}<li class="sl_listen"><a href="/mediaPlayer/#sermonaudio/{{id}}" id="sermon-{{audioid}}">Listen</a></li>{{/audio}}'+
					'{{#download}}<li class="sl_save"><a href="{{download}}">Save</a></li>{{/download}}'+
					'{{#note}}<li class="sl_notes"><a href="{{note}}">Notes</a></li>{{/note}}'+
					'</ul>'+
					'</div>'+
					'{{/sermons}}';

$.getScript('http://storage.churchplantmedia.com/scripts/mustache.js', function(){
    $.getScript('http://storage.churchplantmedia.com/scripts/sermons.plugin.js', function(){
	    $('#sermon-list').sermonify({
	    	headlineElement : false,
	    	template : sermonTemplate,
	    	sorterLabelPosition : 'last',
	    	ajaxCallback : function(){
		    	var text = 'Showing Messages from '+ $('#'+this.settings.currentKey+' option[value="'+this.settings.currentVal+'"]').text();				   
				if(text){
					this.$el.prepend('<h3 id="sl-header">'+text+'</h3>');
				}	
				
				if(this.settings.currentKey == 'series'){
					var frame = this.$el.find('iframe');
					if(frame.length){
						var w = this.$el.width();
						var h = w*(9/16);
						frame.attr('width',w).attr('height',h);
					}
				}
	    	},
		    paginationCallback : function(pagination){
			    var pageHtml = [
                    '<div id="pagination-wrap"><div id="pagination">',
                        '<span>Page:</span>',
                        pagination,
                        '</div>',
                    '</div>'
                ].join('')

                this.$el.append(pageHtml);
				var pagn = $('#pagination');
				var width = pagn[0].offsetWidth;
				var adjust = ( 644 - width)/2;
				pagn.css('margin-left', adjust);
		    }
	    })
    })
});