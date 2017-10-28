jQuery(document).ready(function() {
   // makes styling easier
   $('ul').each(function() {
   $(this).find('li:last-child').addClass('last');
   $(this).find('li:first-child').addClass('first');
   });
   // home page
   $('#home-news .news:last').addClass('last');
   $('.monthwrap').each(function() {
      $(this).find('dl:last-child').addClass('last');
   });
   // Center Pagination
   var pagnWidth = $('#pagination').width()+17;
   var pagnLeft = (608-pagnWidth)/2;
   $('#pagination').css('margin-left', pagnLeft);
   // Event List
   $('#caltabs li a').bind('click', switchCalTabs);
   // Center Category Filter
   var catWidth = $('#cat-filter').width();
   var catLeft = (981-catWidth)/2;
   $('#cat-filter').css('margin-left', catLeft);
   // clear and restore search input
   $('input.clickClear').focus(function() {
      startText = $(this).val();
      $(this).val('');
   });
   $('input.clickClear').blur(function() {
      blurText = $(this).val();
      if (blurText == '') {
         $(this).val(startText);
      };
   });
   // Fix sermon buttons in IE7
   if($.browser.msie && parseFloat($.browser.version) < 8){
      $('.sermon-buttons').each(function() {
         var bWidth = 0;
         $(this).find('li a').each(function() {
            bWidth+=$(this).width();
         });
         bLength = bWidth+7;
         $(this).css('width', bLength);
      });
   }

   	$('#category-filter').bind('change', sortEvents);
   // Make subnav parents intelligent
   $('#subnav li li.current.children').each(function() {
      var kidTest = $(this).find('ul li.current');
      var kidTestLen = kidTest.length;
      if (kidTestLen > 0) {
         $(this).addClass('no-marker');
      };
   });
   
      $('#search a').bind('click', searchSite);
   
});

function switchCalTabs(){
	var div = '#'+$(this).attr('rel');
   if (div=="#events-calendar") {
      $('#events-calendar').show();
      $('#list-view').hide();
   } else {
      $('#list-view').show();
      $('#events-calendar').hide();
   };
	$('#caltabs li').removeClass('current');
	$(this).parent().addClass('current');
	
	sortEvents();
	return false;
}

function searchSite(){

	if($(this).hasClass('close')){
		$(this).removeClass('close');
		$('#searchbar').fadeOut();
	} else {
		$(this).addClass('close');
		$('#searchbar').fadeIn();
	}
	
	$('#close-search').bind('click', function(){
		$('#searchbar').fadeOut();
		$('#search a').removeClass('close');
		return false;
	});
	return false;
}

function sortEvents(){	
	
	// show all calendar items and list items
	$('#events-calendar li, #list-view dl.event-info').fadeIn();
	
	var $self = $('#category-filter');
	
	if($self.find('option:selected').val() > 0){	
	
		// If all events is not chosen, filter
		var $cat = 'category_'+$self.val();			
		$('#events-calendar li:not(.'+$cat+')').fadeOut();
		
		$('#list-view dl.event-info').each(function(){
			if(!$(this).hasClass($cat)){
				$(this).fadeOut();
			}
		});
		
	}		

}

// Added 12.2.11 by RG to add centralized social icons for color customs
// Use foobaralt classes in overrides to add images

socialMediaLinks()
function socialMediaLinks(){
	var social = $('#foot-social');
//	social.find('a').attr('className', '');
	social.find('a[href="#"]').addClass('sharethisalt st_sharethis_custom');
	social.find('a[href*="twitter.com"]').addClass('twitteralt');
	social.find('a[href*="facebook.com"]').addClass('facebookalt');
	social.find('a[href*="youtube.com"]').addClass('youtubealt');
	social.find('a[href*="vimeo.com"]').addClass('vimeoalt');	
	social.find('a').filter('[href^="/feeds/sermons"],[href*="Podcast"],[href*="podcast"]').addClass('rssalt');
	social.find('a').filter('[href^="/feeds/blog"],[href^="/subscriptions"],[href*="blog"],[href*="Blog"]').addClass('rssalt');
	social.find('a:contains("mail")').addClass('ico_emailalt');
	social.filter('[href^="mailto"]').addClass('ico_emailalt');
	$("#foot-social p a:contains('+ Share')").addClass('ico_pill').css('display', 'block');
}

//addThis()
function addThis() {
	$('#foot-social a.sharethisalt').click(function() {
	   return false;
	});
	$('#foot-social a.sharethisalt, .hover_menu').mouseenter(function()
   {
       $('.hover_menu').fadeIn('fast');
       $('.custom_button').addClass('active');
       $(this).data('in', true);
       $('.hover_menu').data('hidden', false);
   }).mouseleave(function()
   {
       $(this).data('in', false);
       setTimeout(hideMenu, delay);
   });

   var delay = 400;
   function hideMenu()
   {
       if (!$('#foot-social a.sharethisalt').data('in') && !$('.hover_menu').data('in') && !$('.hover_menu').data('hidden'))
       {
           $('.hover_menu').fadeOut('fast');
           $('#foot-social a.sharethisalt').removeClass('active');
           $('.hover_menu').data('hidden', true);
       }
   }
}