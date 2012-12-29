/*global module:false*/
module.exports = function(grunt) {

  // Project configuration.
  grunt.initConfig({
    jshint: {
      all: [
        'Gruntfile.js',
        '**/*.js',
        '!node_modules/**',
        '!lib/**'
      ],
      options: {
        curly: true,
        eqeqeq: true,
        immed: true,
        latedef: true,
        newcap: true,
        noarg: true,
        sub: true,
        undef: true,
        boss: true,
        eqnull: true,
        browser: true
      }
    },
    qunit: {
      files: ['test/**/*.html']
    },
    watch: {
      files: '<config:lint.files>',
      tasks: 'lint qunit'
    },
    requirejs: {
      compile: {
        options: {
          appDir: ".",
          baseUrl: "./",
          dir: "min",

          modules: [
            {
              name: "main"
            },
            {
              name: "admin",
              include: [
                "fabric",
                "mapeditor/mapeditor",
                "lib/codemirror/codemirror",
                "lib/codemirror/javascript",
                "lib/codemirror/util/simple-hint",
                "lib/codemirror/util/javascript-hint"
              ],
              exclude: [
                "controller",
                "lib/zepto"
              ]
            }
          ]
        }
      }
    }
  });

  grunt.loadNpmTasks('grunt-contrib-jshint');
  grunt.loadNpmTasks('grunt-contrib-requirejs');
  grunt.registerTask('default', ['jshint']);
};
