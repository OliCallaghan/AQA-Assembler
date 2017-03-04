const fadeout_dur = 2000;
const keywords = ["ldr", "str", "add", "sub", "mov", "cmp", "b", "beq", "bgt", "blt", "bne", "and", "orr", "eor", "mvn", "lsl", "lsr", "halt"];

var fadeouttimer;

function fadeout(t) {
    fadeouttimer = setTimeout(function () {
        $('editor-autocomplete').css('opacity', 0);
        setTimeout(function () {
            $('editor-autocomplete').css('visibility', 'hidden');
        }, 200);
    }, t);
}

function setAutocompletePosition() {
    var code = $('#code');

	var pos = code.caret('pos');
	var offset = code.caret('offset');

    var min = pos - 5;
    if (min < 0) { min = 0; }
	$('editor-autocomplete').css('visibility', 'visible');

	var tokens = code.val().substr(pos - 3, 6).replace(/\n/g, " ").split(' ');
	for (token in tokens) {
		if (keywords.indexOf(tokens[token].toLowerCase()) > -1) {
			$('editor-autocomplete-cmd.selected').removeClass('selected');
			$('editor-autocomplete-cmd[cmd="' + tokens[token].toLowerCase() + '"]').addClass('selected');

			// scroll
			var container = $('editor-autocomplete');
			var scrollTo = $('editor-autocomplete-cmd.selected');

			console.log(tokens[token].toLowerCase());
			container.animate({ scrollTop: scrollTo.offset().top - container.offset().top + container.scrollTop() }, 100);
		}
	}

	clearTimeout(fadeouttimer);
	fadeout(fadeout_dur);

	$('editor-autocomplete').css({
		'top': offset.top - 20 + 'px',
		'left': offset.left + 'px',
		'opacity': 0.95,
		'width': ($('#code').width() - offset.left) + 'px',
		'height': ($('#code').height() - offset.top) + 'px'
	});
	if ($('#code').width() - offset.left < 150) {
		$('editor-autocomplete').css({ 'left': ($('#code').width() - 130) + 'px' });
	}
	if ($('#code').height() - offset.top < 100) {
		$('editor-autocomplete').css({ 'top': (($('#code').height() - 280) + offset.top - ($('#code').height() - 20)) + 'px', 'height':'200px' });
	}
}

function nextCmd() {
	if ($('editor-autocomplete-cmd.selected').nextAll().length == 0) {
		$('editor-autocomplete-cmd.selected').removeClass('selected');
		$('editor-autocomplete-cmd').last().addClass('selected');
	} else {
		$('editor-autocomplete-cmd.selected').removeClass('selected').next().addClass('selected');
	}
}

function prevCmd() {
	if ($('editor-autocomplete-cmd.selected').prevAll().length == 0) {
		$('editor-autocomplete-cmd.selected').removeClass('selected');
		$('editor-autocomplete-cmd').first().addClass('selected');
	} else {
		$('editor-autocomplete-cmd.selected').removeClass('selected').prev().addClass('selected');
	}
}

$('#code').keyup(function (e) {
	if (e.keyCode != 9) {
		setAutocompletePosition();
	}
});

$('#code').keydown(function (e) {
	if (e.keyCode == 9) {
		e.preventDefault();
		clearTimeout(fadeouttimer);
		fadeout(5000);
		if (e.shiftKey) {
			prevCmd();
		} else {
			nextCmd();
		}

	}
});

$('#code').on('scroll', function () {
    if ($('#code').css('visibility') != 'hidden') {
        fadeout(0);
    }
});

$('editor-autocomplete').mouseenter(function () {
    clearTimeout(fadeouttimer);
}).mouseleave(function () {
    fadeout(fadeout_dur);
});
