class TemplateMacros {

    static var current : Template = null;
	static var cachedTemplates : Hash<Template> = new Hash();

	static function getTemplate(n) {
		var t = cachedTemplates.get(n);
		if( t == null ) {
			t = Template.file(n);
			cachedTemplates.set(n,t);
		}
		return t;
	}

}

