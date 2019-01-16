myApp.service('autoSearchFactory', function($http, $q, $window) {
 
  return $http({
        method:"get",
        url:'/789/',
        params:{'symbol':symtext}
    }).then(function(res) {
        return res.data;
      });
 
});