/*global module:false*/
module.exports = function(grunt) {

  // Project configuration.
  grunt.initConfig({
    meta: {
    },
    lint: {
      files: ['grunt.js', 'js/admin/*.js', 'js/controller.js', 'js/main.js', 'js/notifications.js', 'js/util.js']
    },
    qunit: {
      files: ['test/**/*.html']
    },
    watch: {
      files: '<config:lint.files>',
      tasks: 'lint qunit'
    },
    jshint: {
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
      },
      globals: {}
    },
    requirejs: {
      compile: {
        options: {
          appDir: "js/",
          baseUrl: "./",
          dir: "min",

          modules: [
            {
              name: "main"
            },
            {
              name: "admin/admin",
              include: [
                "kinetic",
                "admin/mapeditor",
                "codemirror/codemirror",
                "codemirror/javascript",
                "codemirror/util/simple-hint",
                "codemirror/util/javascript-hint"
              ],
              exclude: [
                "controller"
              ]
            }
          ]
        }
      }
    }
  });

  grunt.registerTask('default', 'lint');
  grunt.loadNpmTasks('grunt-contrib-requirejs');
};
