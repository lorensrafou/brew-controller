using CoreBrewServer.Models;
using Microsoft.AspNetCore.Mvc;
using CoreBrewServer.Helpers;
using CoreBrewServer.Entities;

namespace CoreBrewServer.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class BrewController : Controller
    {
        private readonly BrewingContext _context;
        public BrewController(BrewingContext context){
            _context = context;
        }

        [HttpGet]
        public IActionResult Get()
        {
            Brew status = Brewing.GetBrewStatus();

            return StatusCode(201, status);
        }
    }

}