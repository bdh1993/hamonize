<%@ page language="java" contentType="text/html; charset=UTF-8" pageEncoding="UTF-8"%>
<link rel="stylesheet" type="text/css" href="/css/template/tui-example-style.css" />
<link rel="stylesheet" type="text/css" href="/css/template/docs.css" />
<link rel="stylesheet" type="text/css" href="/css/template/tui-tree.css" />
<link rel="stylesheet" type="text/css" href="/css/groupMenu.css" />

<aside class="lnb">
	
	<!-- tree api : https://nhn.github.io/tui.tree/latest/ -->
	
	<!-- search -->
	<div class="search-container">
		<div class="row form-check-inline">
		  	<a class="btn bg-success btn-md" style="color: white;" id="checkBtn"><i class="fa fa-check" aria-hidden="true"></i> 전체선택</a>     &nbsp;
			<a class="btn btn-md bg-dark" style="color: white;" id="uncheckBtn"><i class="fa fa-check" aria-hidden="true" ></i> 개별선택</a>
		</div>
	</div>
	
	<div class="code-html" style="padding: 0">
		<!-- tree -->
	    <div id="tree" class="tui-tree-wrap"></div>
	</div>
	
	<!-- 체크박스 필요시 사용할 것 -->
	<div class="explain" style="display: none;">
	    <!-- <button id="checkBtn">check</button> -->
	    <!-- <button id="uncheckBtn">uncheck</button> -->
	    <br>
	   <!--  <textarea id="checkedValue"></textarea> -->
	</div>
	
	<script src="/js/template/tui-code-snippet.min.js"></script>
	<script src="/js/template/tui-tree.js"></script>
	<script class="code-js">
	var util = {
	    addEventListener: function(element, eventName, handler) {
	        if (element.addEventListener) {
	            element.addEventListener(eventName, handler, false);
	        } else {
	            element.attachEvent('on' + eventName, handler);
	        }
	    }
	};
	
	// 목록 템플릿 - 데이터는 각각의 페이지에서 전달받도록 구현 필요
	var tree = new tui.Tree('tree', {
	    data: data,
	    nodeDefaultState: 'opened',
	    template: {
	        internalNode:
	            '<div class="tui-tree-content-wrapper" style="padding-left: {{indent}}px">' +
	                '<button type="button" class="tui-tree-toggle-btn tui-js-tree-toggle-btn">' +
	                    '<span class="tui-ico-tree"></span>' +
	                    '{{stateLabel}}' +
	                '</button>' +
	                '<span class="tui-tree-text tui-js-tree-text groupLabel" data-value="{{step}}" data-upcode="{{uppercode}}" data-code="{{code}}" data-orgseq="{{seq}}" data-groupgubun="{{groupgubun}}">' +
	                    '<label class="tui-checkbox">' +
	                	   '<span class="tui-ico-check"><input type="checkbox" class="tui-tree-checkbox"></span>' +
	                	'</label>' +
	                    '<span class="tui-tree-ico tui-ico-folder"></span>' +
	                    '{{text}}' +
	                '</span>' +
	            '</div>' +
	            '<ul class="tui-tree-subtree tui-js-tree-subtree">{{children}}</ul>',
	        leafNode:
	            '<div class="tui-tree-content-wrapper" style="padding-left: {{indent}}px">' +
	                '<span class="tui-tree-text tui-js-tree-text node groupLabel" data-value="{{step}}" data-upcode="{{uppercode}}" data-code="{{code}}" data-orgseq="{{seq}}" data-groupgubun="{{groupgubun}}">' +
	                    '<label class="tui-checkbox">' +
	                       '<span class="tui-ico-check"><input type="checkbox" class="tui-tree-checkbox"></span>' +
	                    '</label>' +
	                    	'<span class="tui-tree-ico tui-ico-file"></span>' +		
	                    	'{{groupgubun}}' +'{{text}}' +   
	                '</span>' +
	            '</div>'
	    }
	}).enableFeature('Checkbox', {
	    checkboxClassName: 'tui-tree-checkbox',
	});
	
	
	
	// 동적 할당
	var checkBtn = document.getElementById('checkBtn');
	var uncheckBtn = document.getElementById('uncheckBtn');
	var checkedValue = document.getElementById('checkedValue');
	var rootNodeId = tree.getRootNodeId();
	var firstChildId = tree.getChildIds(rootNodeId)[0];
	tree.on('check', function(eventData) {
	    var nodeData = tree.getNodeData(eventData.nodeId);
	    //checkedValue.value = 'checked : ' + nodeData.text;
	});
	tree.on('uncheck', function(eventData) {
	    var nodeData = tree.getNodeData(eventData.nodeId);
	    //checkedValue.value = 'unchecked : ' + nodeData.text;
	});
	util.addEventListener(checkBtn, 'click', function() {
	    tree.check(firstChildId);
//	    $('#tree .tui-checkbox').css('display', '');
	    $('#pageGrideInSgbList').parent('.inner-wrap').css('display', 'none');	// 사지방목록
	    $('#pageGrideInSgbList').parent('.inner-wrap').prev('.section').css('display', 'none');	// 사지방목록 텍스트
	    $('#pageGrideInSgbList').empty().append('등록된 사지방 정보가 없습니다.');
	});
	util.addEventListener(uncheckBtn, 'click', function() {
	    tree.uncheck(firstChildId);
//	    $('#tree .tui-checkbox').css('display', 'none');
	    $('#pageGrideInSgbList').parent('.inner-wrap').css('display', '');	// 사지방목록
	    $('#pageGrideInSgbList').parent('.inner-wrap').prev('.section').css('display', '');	// 사지방목록 텍스트
	    $('#pageGrideInSgbList').empty().append('등록된 사지방 정보가 없습니다.');
	});
	</script>
</aside>