/* 
	High Five skin by Laszlo Molnar
	(C) 2011
*/

var DEBUG = true,
	UNDEF = 'undefined',
	OBJECT = 'object',
	NOLINK = 'javascript:void(0)',
	LOCAL = location.protocol.indexOf('file:') === 0;

	String.prototype.trim = function() { 
		return this.replace(/^\s+|\s+$/g,''); 
	};
	
	String.prototype.trunc = function( n ) {
		if (this.length <= n) {
			return this.toString();
		}
		var s = this.substring(0, n - 1), i = s.lastIndexOf(' ');
		return ((i > 6 && (s.length - i) < 20)? s.substring(0, i) : s) + '...';
	};
	
	String.prototype.startsWith = function( s ) {
		return this.indexOf( s ) === 0;
	};
	
	String.prototype.endsWith = function( s ) {
		return this.substring(this.length - s.length) === s;
	};
	
	String.prototype.getExt = function() {
		var i = this.lastIndexOf('.');
		return (i <= 0 || i >= this.length - 1)? '' : this.substring(i + 1).toLowerCase();
	};
	
	String.prototype.replaceExt = function( s ) {
		var i = this.lastIndexOf('.');
		return (i <= 0)? this : (this.substring(0, i + 1) + s);  
	};
	
	String.prototype.fixUrl = function() {
		var i, j, s = this + '';
		while ( (i = s.indexOf('../')) > 0) {
			if ( i === 1 || (j = s.lastIndexOf('/', i - 2)) === -1 ) {
				return s.substring(i + 3);
			}
			s = s.substring(0, j) + s.substring(i + 2);
		}
		return s;
	};

/*	
 *	addPlayer() :: adds jPlayer video player component
 *
 *	author: Laszlo Molnar (c) 2013
 *
 *	Usage: $(element).addPlayer( options, text )
 *	Options:
		id: 'jp_container_',			// ID for the container element
		backgroundColor: '#000000',		// Background color
		resPath: '',					// Path to 'res' folder
		swf: 'Jplayer.swf',				// Name of the SWF player
		relativeUrl: false,				// Allow using relative URLs
		solution: 'html,flash',			// Priority
		auto: false,					// Auto start
		loop: false,					// Loop playback
		keyboard: true,					// Use "space" key for play toggle
		size: {							// Player size
			width: '100%',
			height: '100%'
		}
 */

(function($) {
		
	$.fn.addPlayer = function( settings, text ) {
		
		settings = $.extend( {}, $.fn.addPlayer.defaults, settings );
		text = $.extend( {}, $.fn.addPlayer.text, text );
		
		if ( typeof $.fn.jPlayer === UNDEF ) {
			return;
		}
		
		// Class names
				
		var sel = {
			cont: 'jp-cont',
			mini: 'jp-mini',
			audio: 'jp-audio',
			video: 'jp-video',
			playerType: 'jp-type-single',
			player: 'jp-jplayer',
			title: 'jp-title',
			progress: 'jp-progress',
			controls: 'jp-controls-holder',
			startStop: 'jp-startstop',
			volume: 'jp-volume',
			times: 'jp-times',
			toggles: 'jp-toggles',
			warning: 'jp-warning',
			// defined in jPlayer
			videoPlay: 'jp-video-play',
			play: 'jp-play',
			pause: 'jp-pause',
			stop: 'jp-stop',
			seekBar: 'jp-seek-bar',
			playBar: 'jp-play-bar',
			mute: 'jp-mute',
			unmute: 'jp-unmute',
			volumeBar: 'jp-volume-bar',
			volumeBarValue: 'jp-volume-bar-value',
			volumeMax: 'jp-volume-max',
			currentTime: 'jp-current-time',
			duration: 'jp-duration',
			fullScreen: 'jp-full-screen',
			restoreScreen: 'jp-restore-screen',
			repeat: 'jp-repeat',
			repeatOff: 'jp-repeat-off',
			gui: 'jp-gui',
			noSolution: 'jp-no-solution',
			playing: 'playing'
		};
		
		// Compiling interface
		
		var getInterface = function( audio ) {
			var html;
			
			var adda = function(name) {
				return '<a class="'+sel[name]+'" title="'+text[name]+'">'+text[name]+'</a>';
			};
			
			// Progress bar
			html = '<div class="'+sel.progress+'"><div class="'+sel.seekBar+'"><div class="'+sel.playBar+'"></div></div></div>';
			
			// Controls
			html += '<div class="'+sel.controls+'">';
			
				// Start/Stop (Prev/Next)
				html += '<div class="'+sel.startStop+'">' + adda('play') + adda('pause') + adda('stop') + '</div>';
				
				// Volume
				html += '<div class="'+sel.volume+'">' + adda('mute') + adda('unmute') + 
					'<div class="'+sel.volumeBar+'"><div class="'+sel.volumeBarValue+'"></div></div>' + '</div>';
					
				// Times: Current | Total
				html += '<div class="'+sel.times+'"><div class="'+sel.currentTime+'"></div><div class="'+sel.duration+'"></div></div>';
				
				// Toggle buttons
				html += '<div class="'+sel.toggles+'">' + (audio? '' : (adda('fullScreen') + adda('restoreScreen'))) +
					adda('repeat') + adda('repeatOff') + '</div>';
			
			html += '</div>';
			return html;
		};
		
		// Fix gui to match to container's padding
		
		var fixPadding = function( c ) {
			var pt = c.css('paddingTop'),
				pl = c.css('paddingLeft'),
				pr = c.css('paddingRight'),
				pb = c.css('paddingBottom');
				
			c.find('.' + sel.gui).css({
				bottom: pb,
				left: pl,
				right: pr
			});
			c.find('.' + sel.title).css({
				top: pt,
				left: pl,
				right: pr
			});
		};
		
		// Compiling GUI
		
		var createPlayer = function( to, title, audio ) {
			
			// Required to be able to use absolute positioned GUI elements
			if ( to.css('position') !== 'absolute' && to.css('position') !== 'fixed' ) {
				to.css({
					position: 'relative'
				});
			}
			
			to.css({
				overflow: 'hidden'
			});
			
			var pl, el = $('<div class="' + (audio? sel.audio : sel.video) + '"></div>').appendTo(to);

			// Player type wrap element
			el = $('<div class="' + sel.playerType + '"></div>').appendTo(el);
			
			// Adding player box
			pl = $('<div class="' + sel.player + '"></div>').appendTo(el);
			
			// Play button overlay
			el.append('<div class="' + sel.videoPlay + '"><a>' + text.play + '</a></div>');
			
			// Title
			if ( title ) {
				el.append('<div class="' + sel.title + '"><ul><li>' + title + '</li></ul></div>');
			}
			
			// Interface
			el.append('<div class="' + sel.gui + '">' + getInterface( audio ) + '</div>');
			
			// Adding "javascript:void" links to buttons
			el.find('.' + sel.gui + ' a').attr('href', NOLINK);
			
			// No solution layer
			to.append('<div class="' + sel.noSolution + '">' + text.noSolution + '</div>');

			// Fix padding
			fixPadding( to );
										
			return pl;
		};
		
		// Pause request
		
		var pauseFn = function() {
			var p;
			if ( (p = $(this).data('media')) ) {
				p.jPlayer('pause');
			}
			return false;
		};
		
		// Destroy request
		
		var destroyFn = function() {
			var p;
			if ( (p = $(this).data('media')) ) {
				p.jPlayer('destroy');
			}
			$(window).off('keydown', keyhandler);
			return false;
		};
		
		// Stop request
		
		var stopFn = function() {
			var p;
			if ( (p = $(this).data('media')) ) {
				p.jPlayer('stop');
			}
			return false;
		};
		
		// Play request
		
		var playFn = function() {
			var p;
			if ( (p = $(this).data('media')) ) {
				p.jPlayer('play');
			}
			return false;
		};
		
		// Keyboard handler hooked to the first media player element
		
		var firstPlayer = $(this).eq(0);
		var keyhandler = function(e) {
			if ( (document.activeElement && (document.activeElement.nodeName === 'INPUT' || 
					document.activeElement.nodeName === 'TEXTAREA')) ) {
				return true;
			}
		
			var k = e? e.keyCode : window.event.keyCode;
			
			if ( k === 32 ) {
				firstPlayer.find('.' + sel.player).jPlayer( firstPlayer.data(sel.playing)? 'pause' : 'play' );
				return false;
			}
			return true;
		};
		
		// Check Audio
		
		var checkAudio = function( src ) {
			return settings.hasOwnProperty('audio') ? settings.audio : ('.mp3.m4a.f4a.rtmpa'.indexOf(src.getExt()) > 0);
		};
		
		// Check if any playing is on
		
		var checkAnyPlay = function() {
			$('.' + sel.cont).each(function() {
				if ( $(this).data(sel.playing) ) {
					return true;
				}
			});
			return false;
		};
				
		// Get the media format...
		
		var getFormat = function( src ) {
			
			// Finding or guessing the format
			var format,
				av =  checkAudio( src )? 'a' : 'v';
			
			switch (src.getExt()) {
				case 'mp3':
					format = 'mp3';
					break;
				case 'mp4': 
					format = 'm4' + av;
					break;
				case 'ogg': 
					format = 'og' + av;
					break;
				case 'webm':
					format = 'webm' + av;
					break;
				case 'flv':
				case 'f4a':
				case 'f4v':
					format = 'fl' + av;
					break;
				case 'rtmp':
					format = 'rtmp' + av;
					break;
				default:
					format = null;
			}
			
			return format;
		};
		
		// Main loop
		
		return this.each(function() {
			
			var cont = $(this),
				audio,
				format,
				enableAuto,
				autoHide,
				folder = settings.folder || '',
				id, src, title, poster, elem, pl, curr = 0;
						
			if ( settings.elem ) {
				
				// Reading source, title and poster from a link element
				elem = $(this).find(settings.elem);
				src = elem.attr('href');
				title = elem.attr('title');
				var img = elem.find('img:first');
				if ( img.length ) {
					poster = img.attr('src');
					if ( !title ) {
						title = img.attr('alt');
					}
				} else {
					poster = title = '';
				}
				
			} else {
				
				// Provided through call parameters
				src = settings.src;
				title = settings.title || '';
				poster = settings.poster || '';
				
				elem = $('<a href="' + src + '"' +
					(title? (' title="' + title + '"') : '') + 
					'>' + (poster? ('<img src="' + poster + '">') : '') + 
					'</a>').appendTo($(this));
			}
			
			// Local Flash warning
			if ( LOCAL ) {
				var w = $('<div class="' + sel.warning + '">' + text.localFlashWarning + '</a></div>').appendTo(elem);
				elem.css('position', 'relative');
				w.hide();
				setTimeout(function() {
					w.fadeIn();
				}, 2000);
			}			

			if ( !src ) {
				return;
			}
			
			// Can the media auto started?
			enableAuto = settings.auto && !settings.lowPriority && !checkAnyPlay();
			
			// Playlist?
			if ( src.indexOf('::') > 0 ) {
				src = src.split('::');
				// Checking the first element
				audio = checkAudio( src[0] );
				format = getFormat( src[0] );
			} else {
				audio = checkAudio( src );
				format = getFormat( src );
			}
			
			// Adding or reading container id
			if ( !this.id ) {
				this.id = settings.id + $.fn.addPlayer.id++;
			}	
			id = '#' + this.id;
			
			$(cont).addClass(sel.cont);
			if ( settings.mini ) {
				$(cont).addClass(sel.mini);
			}
			
			// Creating the structure
			pl = createPlayer( cont, title, audio );
			
			// Getting current media
			var getMedia = function() {
				
				// Media URL
				var sm = {},
					csrc,
					format,
					base = settings.relativeUrl? '' : location.href.substring(0, location.href.lastIndexOf('/') + 1);
				
				if ( $.isArray( src ) ) {
					if ( curr >= src.length ) {
						curr = 0;
					}
					// Playlist
					csrc = src[curr];
				} else {
					csrc = src;
					if ( poster ) {
						sm.poster = (base + folder + poster).fixUrl();
					}
				}
				
				format = getFormat(csrc);
				sm[format] = (base + folder + csrc).fixUrl();
				
				return sm;
				
			};
			
			// Saving play status
			
			var saveStatus = function() {
				if ( $.cookie ) {
					var tm = cont.find('.' + sel.currentTime).text().split(':');
					if (tm.length > 2 ) {
						tm = (parseInt(tm[0], 10) * 60 + parseInt(tm[1], 10)) * 60 + parseInt(tm[2], 10);
					} else {
						tm = parseInt(tm[0], 10) * 60 + parseInt(tm[1], 10);
					}
					$.cookie('jp_' + cont[0].id, (cont.data(sel.playing)? '1':'0') + 
						'::' + tm + 
						'::' + pl.jPlayer('option', 'volume').toString().substring(0,5) +
						((curr !== UNDEF)? ('::' + curr) : '')
					);
				}
			};
			
			// Loading play status
			
			var loadStatus = function(el) {
				if ( $.cookie ) {
					var c = $.cookie('jp_' + cont[0].id);
					if ( c ) {
						c = c.split('::');
						return { 
							playing: c[0] === '1',
							time: parseInt(c[1] || 0, 10),
							volume: parseFloat(c[2] || 0.8),
							curr: parseInt(c[3] || 0, 10)
						};
					}
				}
				return null;
			};
			
			// Auto hide if not audio and not Firefox on Mac (fixing a bug)
			autoHide = !audio && !( $.browser.mozilla && navigator.platform.indexOf('Mac') >= 0 );
			
			// Calling jPlayer
			pl.jPlayer({
				cssSelectorAncestor: id,
				backgroundColor: settings.backgroundColor,
				supplied: format,
				swfPath: settings.resPath + '/' + settings.swf,
				solution: settings.solution,
				size: {
					width: '100%',
					height: '100%'
				},
				preload: 'auto',
				loop: settings.loop,
				volume: settings.volume,
				autohide: {
					restored: autoHide,
					full: autoHide
				},
				ready: function() {
					
					var t = $(this),
						st = settings.saveStatus? loadStatus(cont) : null;
					
					// Saving reference to player in the container element
					cont.data('media', t);
					
					// Save status on unload, set current
					if ( settings.saveStatus ) {
						$(window).on('unload', saveStatus);	
						if ( st ) {
							curr = st.curr;
						}
					}
					
					// Setting media source
					var sm = getMedia();
					
					// Hiding original poster element, showing GUI
					if ( elem ) {
						elem.hide();
					}
					cont.find('.' + sel.gui).show();
					
					t.jPlayer('setMedia', sm);
										
					// Adding events to container
					cont.on({
						play: playFn,
						pause: pauseFn,
						stop: stopFn,
						destroy: destroyFn
					});
					
					// Auto start
					if ( settings.saveStatus && st ) {
						t.jPlayer('volume', st.volume);
						t.jPlayer(st.playing? 'play' : 'pause', st.time);
					} else if ( settings.auto ) {
						t.jPlayer('play');
					}
					
					// Key handler
					if ( !settings.lowPriority ) {
						$(window).on('keydown', keyhandler);
					}
				},
				// Playing indicator on container element
				play: function() { 
					// Avoid other jPlayers playing together
					$(this).jPlayer('pauseOthers');
					cont.data(sel.playing, true);
				},
				pause: function() {
					cont.data(sel.playing, false);
				},
				stop: function() {
					cont.data(sel.playing, false);
				},
				ended: function() {
					if ( $.isArray(src) && ((curr + 1) < src.length || settings.loop) ) {
						curr = (curr + 1) % src.length;
						$(this).jPlayer('setMedia', getMedia());
						if ( settings.auto ) {
							$(this).jPlayer('play');
						}
					} else {
						cont.data(sel.playing, false);
						if ( $.isFunction(settings.ended) ) {
							settings.ended.call();
						}
					}
				}
			});
		});
	};
	
	$.fn.addPlayer.id = 0;

	$.fn.addPlayer.defaults = {
		id: 'jp_container_',
		backgroundColor: '#000000',
		resPath: '',
		swf: 'Jplayer.swf',
		relativeUrl: false,
		solution: 'html,flash',
		volume: 0.8,
		auto: false,
		loop: false,
		keyboard: true,
		lowPriority: false,
		saveStatus: false,
		mini: false,
		size: {
			width: '100%',
			height: '100%'
		}
	};
	
	$.fn.addPlayer.text = {
		play: 'play',
		pause: 'pause',
		stop: 'stop',
		mute: 'mute',
		unmute: 'unmute',
		fullScreen: 'full screen',
		restoreScreen: 'restore screen',
		repeat: 'repeat',
		repeatOff: 'repeat off',
		localFlashWarning: 'Local Flash playback is possibly blocked by Flash security rules. Test videos in the uploaded album!', 
		noSolution: '<span>Unsupported media format</span>You might need to either update your browser or the <a href="http://get.adobe.com/flashplayer/" target="_blank">Flash plugin</a> or use another browser to play this media.'
	};

})(jQuery);

/*	
 *	alignTo() :: align a layer to another
 *
 *	Usage: $(element).alertBox( target, options);
 *	options: gap, posX, posY, toX, toY
 */

(function($) {
		
	var ALIGN_LEFT = 0,  ALIGN_TOP = 0,
		ALIGN_CENTER = 1, ALIGN_MIDDLE = 1,
		ALIGN_RIGHT = 2, ALIGN_BOTTOM = 2;
	
	$.fn.alignTo = function( el, settings ) {
		
		settings = $.extend( {}, $.fn.alignTo.defaults, settings );
		
		if (typeof el === 'string') {
			el = $(el);
		}
		if (!(el instanceof $ && el.length)) {
			return;
		}
		
			
		var to = el.offset(),
			tw = el.outerWidth(),
			th = el.outerHeight();
		
		return $(this).each( function() {
			var w = $(this).outerWidth(),
				h = $(this).outerHeight(),
				rx = Math.round(to.left + settings.toX * tw / 2 + 
					(settings.toX - 1) * settings.gap),
				ry = Math.round(to.top + settings.toY * th / 2 + 
					(settings.toY - 1) * settings.gap),
				l = Math.round(rx - settings.posX * w / 2),
				t = Math.round(ry - settings.posY * h / 2);
			
			if ( t < 0 ) {
				if ( settings.toX !== ALIGN_CENTER ) {
					t = 0;
				} else if ( settings.toY !== ALIGN_BOTTOM  ) {
					t = to.top + el.outerHeight() + settings.gap;
				}
			} else if ( (t + h) > $(window).height() ) {
				if ( settings.toX !== ALIGN_CENTER ) {
					t = $(window).height() - h;
				} else if ( settings.toY !== ALIGN_TOP ) {
					t = to.top - h - settings.gap;
				}
			}
			
			if ( l < 0 ) {
				if ( settings.toY !== ALIGN_MIDDLE ) {
					l = 0;
				} else if ( settings.toX !== ALIGN_RIGHT ) {
					l = to.left + el.outerWidth() + settings.gap;
				}
			} else if ( (l + w) > $(window).width() ) {
				if ( settings.toY !== ALIGN_MIDDLE ) {
					l = $(window).width() - w;
				} else if ( settings.toX !== ALIGN_LEFT ) {
					l = to.left - w - settings.gap;
				}
			}
			
			$(this).css({
				position: 'absolute',
				left: l, 
				top: t 
			});
		});
	};

	$.fn.alignTo.defaults = {
		gap: 5,
		posX: ALIGN_CENTER,
		posY: ALIGN_BOTTOM,
		toX: ALIGN_CENTER,
		toY: ALIGN_TOP
	};
	
})(jQuery);

/*	
 *	addHint() :: little Popup displaying 'title' text, or passed text (can be HTML)
 *
 *	Usage: $(element).addHint( [txt,] options );
 *	options:
		id: 'hint',
		stay: 3000,
		posX: ALIGN_CENTER,
		posY: ALIGN_BOTTOM,
		toX: ALIGN_CENTER,
		toY: ALIGN_TOP
 */

(function($) {
			
	$.fn.addHint = function(txt, settings) {
		
		if ( txt && typeof txt !== 'string' && !txt.jquery ) {
			settings = txt;
			txt = null;
		}

		settings = $.extend( {}, $.fn.addHint.defaults, settings );
		
		var getPop = function() {
			var c = $('#' + settings.id);
			if ( !c.length ) {
				c = $('<div>', { 
					'class': settings.id, 
					id: settings.id 
				}).hide().appendTo('body');
			}
			return c;
		};
		
		return this.each(function() {
			var t = $(this), 
				tx = txt || t.attr('title'), 
				to, 
				over = false,
				focus = false,
				dyn = !(tx && tx.jquery), 
				pop;
			
			if ( !tx || !tx.length ) {
				return;
			}
			
			var enter = function() {
				// Inserting dynamic content
				if ( dyn ) {
					pop = getPop();
					pop.empty().html( tx );
				} else {
					pop = tx.show();
				}
				
				pop.off('mouseover', getFocus);
				pop.off('mouseout', lostFocus);
				
				// getFocus, lostFocus
				var getFocus =  function() {
					to = clearTimeout(to);
					over = true;
					pop.stop(true, true).css({opacity: 1}).show();
				};
				var lostFocus = function() {
					if ( focus ) {
						return;
					}
					to = clearTimeout(to);
					over = false;
					fade();
				};
				
				// Keep the popup live while the mouse is over, or an input box has focus
				pop.on('mouseover', getFocus);
				pop.on('mouseout', lostFocus);
				pop.find('input').on({
					focus: function() {
						focus = true;
						getFocus();
					},
					blur: function() {
						focus = false;
					}
				});
				
				// Aligning and fading in
				pop.stop(true, true).alignTo(t, { 
					posX: settings.posX,
					posY: settings.posY,
					toX: settings.toX,
					toY: settings.toY 
				});
				pop.css({
					opacity: 0
				}).show().animate({ 
					opacity: 1 
				}, 200);
				
				// Remove hint automatically on touch devices, because there's no explicit mouse leave event is triggered
				if ( document.touchMode ) {
					to = setTimeout(fade, settings.stay);
				} else {
					over = true;
				}
			};
			
			// Leaving the trigger element
			var leave = function() {
				over = false;
				to = clearTimeout(to);
				fade();
			};
			
			// Custom event = Force removing the hint
			t.on('removeHint', leave);
			
			// Fading the popup
			var fade = function() {
				if ( !over && pop && pop.length ) {
					pop.stop(true, false).animate({
						opacity: 0
					}, 200, function() { 
						$(this).hide(); 
					});
				}
			};
			
			if ( tx.jquery ) {
				tx.addClass( settings.id );
			} else {
				t.removeAttr('title');
			}
			
			t.on(document.touchMode? {
				'touchstart': enter
			} : {
				'focus mouseenter': enter,
				'blur mouseleave': leave
			});
		});
	};
	
	/*
		ALIGN_LEFT = ALIGN_TOP = 0
		ALIGN_CENTER = ALIGN_MIDDLE = 1
		ALIGN_RIGHT = ALIGN_BOTTOM = 2
	*/
	$.fn.addHint.defaults = {
		id: 'hint',
		stay: 3000,
		posX: 1,
		posY: 2,
		toX: 1,
		toY: 0
	};
	
})(jQuery);
