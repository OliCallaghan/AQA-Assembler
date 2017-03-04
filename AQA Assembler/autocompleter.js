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

	var min = pos - 4;
	if (min < 0) { min = 0; }
	var tokens = code.val().substr(min, 8).replace(/\n/g, " ").split(' ');
	for (token in tokens) {
		if (keywords.indexOf(tokens[token].toLowerCase()) > -1) {
			$('editor-autocomplete-cmd.selected').removeClass('selected');
			$('editor-autocomplete-cmd[cmd="' + tokens[token].toLowerCase() + '"]').addClass('selected');

			scrollToAutocompleteHighlighted();
			break;
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

function scrollToAutocompleteHighlighted() {
	// Scroll to selected autocomplete
	var container = $('editor-autocomplete');
	var scrollTo = $('editor-autocomplete-cmd.selected');

	container.animate({
		scrollTop: scrollTo.offset().top - container.offset().top + container.scrollTop()
	}, 100);
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

var constrict_tab = false;

$('#code').keydown(function (e) {
	if (e.keyCode == 9) {
		e.preventDefault();
		if (constrict_tab == true) {
			fillTemplate();
		} else {
			if ($('editor-autocomplete').css('visibility') != 'visible') {
				setAutocompletePosition();
			} else {
				clearTimeout(fadeouttimer);
				fadeout(5000);
				if (e.shiftKey) {
					prevCmd();
				} else {
					nextCmd();
				}
				scrollToAutocompleteHighlighted();
			}
		}
	} else if (e.keyCode == 13) {
		if (e.shiftKey) {
			e.preventDefault();
			insert($('editor-autocomplete-cmd.selected'));
		}
	} else if (e.keyCode <= 40 && e.keyCode >= 37) {
		constrict_tab = false;
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

function insert(obj) {
	var code = $('#code');
	var pos = code.caret('pos');

	var insert = obj.children('cmd-eg').text();
	last_inst_template = obj.children('cmd-eg').text();
	var code_val = code.val();

	var pos_min = pos - 1;
	while (pos_min >= 0 && code_val.substr(pos_min, 1) != '\n') {
		pos_min -= 1;
	}

	var pos_max = pos - 1;
	while (pos_max < code_val.length && code_val.substr(pos_max, 1) != '\n') {
		pos_max += 1;
	}

	var front = code_val.substring(0,pos_min + 1);
	var back = code_val.substring(pos_max, code_val.length);

	code.val(front + insert + back);

	var regex = /<\w+>/g;
	var selections = [];
	while ((match = regex.exec(insert)) != null) {
		selections.push(match);
	}

	constrict_tab = true;

	if (selections.length > 0) {
		document.getElementById('code').setSelectionRange(pos_min + selections[0].index + 1, pos_min + selections[0].index + selections[0][0].length + 1);
	} else {
		code.caret('pos', pos_max);
		constrict_tab = false;
	}
}

function fillTemplate() {
	var code = $('#code');
	var pos = code.caret('pos');
	var code_val = code.val();

	var pos_min = pos - 1;
	while (pos_min >= 0 && code_val.substr(pos_min, 1) != '\n') {
		pos_min -= 1;
	}

	var pos_max = pos - 1;
	while (pos_max < code_val.length && code_val.substr(pos_max, 1) != '\n') {
		pos_max += 1;
	}

	var regex = /<\w+>/g;
	var selections = [];
	while ((match = regex.exec(code_val.substring(pos_min, pos_max))) != null) {
		selections.push(match);
	}

	if (selections.length > 0) {
		if (pos_min <= 0) {
			document.getElementById('code').setSelectionRange(pos_min + selections[0].index + 1, pos_min + selections[0].index + selections[0][0].length + 1);
		} else {
			document.getElementById('code').setSelectionRange(pos_min + selections[0].index, pos_min + selections[0].index + selections[0][0].length);
		}
	} else {
		code.caret('pos', pos_max);
		constrict_tab = false;
	}
}

$('editor-autocomplete-cmd').click(function () {
	insert($(this));

	setAutocompletePosition();
	clearTimeout(fadeouttimer)
});
